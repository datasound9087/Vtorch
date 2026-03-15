#include "Swapchain.h"
#include "ImageFormat.h"
#include "LoggingLib/Logging.h"
#include <algorithm>
#include <ranges>

Swapchain::Swapchain(Context &context, GLFWwindow *window) : m_context(context)
{
    LOG_DEBUG("Creating swapchain");
    const auto swapchainInfo = context.GetSwapchainInfo();

    const auto formatIt = std::ranges::find_if(
        swapchainInfo.formats,
        [](const auto &format)
        {
            // SRGB results in more accurately percieved colours
            // GPUs tend to order pixels as BGRA, so use that
            return format.format == vk::Format::eB8G8R8A8Srgb &&
                   format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });

    if (formatIt == swapchainInfo.formats.end())
    {
        throw std::runtime_error(
            "Failed to find supported image format for swapchain");
    }

    m_format = formatIt->format;

    const bool tripleBuffering =
        std::ranges::any_of(swapchainInfo.presentModes, [](const auto value)
                            { return value == vk::PresentModeKHR::eMailbox; });

    if (!tripleBuffering)
    {
        throw std::runtime_error(
            "Triple buffering not supported for swapchain");
    }

    int width{};
    int height{};
    glfwGetFramebufferSize(window, &width, &height);

    m_swapExtent = {.width = std::clamp<uint32_t>(
                        static_cast<uint32_t>(width),
                        swapchainInfo.capabilities.minImageExtent.width,
                        swapchainInfo.capabilities.maxImageExtent.width),
                    .height = std::clamp<uint32_t>(
                        static_cast<uint32_t>(height),
                        swapchainInfo.capabilities.minImageExtent.height,
                        swapchainInfo.capabilities.maxImageExtent.height)};

    const uint32_t imageCount =
        // When maxImageCount is zero, there is no max image count
        swapchainInfo.capabilities.maxImageCount == 0
            ? 3u
            // Make sure that the vlue is between minImageCount and
            // maxImageCount
            : std::clamp<uint32_t>(swapchainInfo.capabilities.minImageCount + 1,
                                   swapchainInfo.capabilities.minImageCount,
                                   swapchainInfo.capabilities.maxImageCount);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = swapchainInfo.surface,
        .minImageCount = imageCount,
        .imageFormat = formatIt->format,
        .imageColorSpace = formatIt->colorSpace,
        .imageExtent = m_swapExtent,
        .imageArrayLayers = 1,
        // Currently rendering directly to the swapchain. May be
        // vk::ImageUsageFlagBits::eTransferDst at some point
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        // An image is owned by one queue family at once
        .imageSharingMode = vk::SharingMode::eExclusive,
        // Do not transform the images (i.e. rotation)
        .preTransform = swapchainInfo.capabilities.currentTransform,
        // Don't blend alpha channel
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = vk::PresentModeKHR::eMailbox,
        // Don't care about obscured pixels
        .clipped = true,
        // Old swapchain (don't have one yet)
        .oldSwapchain = nullptr};

    m_swapChain =
        vk::raii::SwapchainKHR(context.GetDevice(), swapChainCreateInfo);
    m_swapChainImages = m_swapChain.getImages();

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = formatIt->format,
        // components can be used for channel swizzling/remappng
        // .components = ...,
        // Purpose and what will be accessed
        .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                             // No mipmapping or multiple layers
                             .levelCount = 1,
                             .layerCount = 1}};

    for (auto &image : m_swapChainImages)
    {
        imageViewCreateInfo.image = image;
        m_swapChainImageViews.emplace_back(context.GetDevice(),
                                           imageViewCreateInfo);
    }

    // Init render resources for each frame
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = context.GraphicsIndex()};
    m_commandPool = vk::raii::CommandPool(context.GetDevice(), poolInfo);

    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = m_commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MaxFramesInFlight};
    m_commandBuffers = vk::raii::CommandBuffers(context.GetDevice(), allocInfo);

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        m_renderFinishedSemaphores.emplace_back(context.GetDevice(),
                                                vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < MaxFramesInFlight; i++)
    {
        m_presentCompleteSemaphores.emplace_back(context.GetDevice(),
                                                 vk::SemaphoreCreateInfo());
        m_inFlightFences.emplace_back(
            context.GetDevice(),
            vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
    }
}

const vk::Format &Swapchain::GetFormat() const { return m_format; }

FrameInfo Swapchain::BeginFrame()
{
    auto &cmdBuffer = m_commandBuffers[m_frameIndex];
    auto &fence = m_inFlightFences[m_frameIndex];
    auto &presentCompleteSemaphore = m_presentCompleteSemaphores[m_frameIndex];

    const auto &device = m_context.GetDevice();
    const auto fenceResult =
        m_context.GetDevice().waitForFences(*fence, vk::True, UINT64_MAX);

    if (fenceResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to wait for fence!");
    }
    device.resetFences(*fence);

    auto [result, imageIndex] = m_swapChain.acquireNextImage(
        UINT64_MAX, *presentCompleteSemaphore, nullptr);
    m_imageIndex = imageIndex;
    auto &image = m_swapChainImages[m_imageIndex];
    auto &imageView = m_swapChainImageViews[m_imageIndex];

    cmdBuffer.reset();
    cmdBuffer.begin({});

    // Transition swapchain image to useable format
    renderer::vulkan::TransitionImageLayout(
        cmdBuffer, image, vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal, {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView = imageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor};

    vk::RenderingInfo renderingInfo = {
        .renderArea = {.offset = {0, 0}, .extent = m_swapExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo};

    cmdBuffer.beginRendering(renderingInfo);

    return FrameInfo{.commandBuffer = &cmdBuffer};
}

void Swapchain::EndFrame()
{
    auto &cmdBuffer = m_commandBuffers[m_frameIndex];
    auto &image = m_swapChainImages[m_imageIndex];
    auto &fence = m_inFlightFences[m_frameIndex];
    auto &presentCompleteSemaphore = m_presentCompleteSemaphores[m_frameIndex];
    auto &renderFinishedSemaphore = m_renderFinishedSemaphores[m_imageIndex];

    cmdBuffer.endRendering();

    renderer::vulkan::TransitionImageLayout(
        cmdBuffer, image, vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite, {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe);
    cmdBuffer.end();

    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &(*presentCompleteSemaphore),
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &(*cmdBuffer),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &(*renderFinishedSemaphore)};

    const auto &queue = m_context.GraphicsQueue();
    queue.submit(submitInfo, *fence);

    const vk::PresentInfoKHR presentInfoKHR{.waitSemaphoreCount = 1,
                                            .pWaitSemaphores =
                                                &(*renderFinishedSemaphore),
                                            .swapchainCount = 1,
                                            .pSwapchains = &(*m_swapChain),
                                            .pImageIndices = &m_imageIndex};

    const auto presentResult = queue.presentKHR(presentInfoKHR);
    if (presentResult == vk::Result::eSuboptimalKHR)
    {
        LOG_WARN("Suboptimal KHR returned, swapchain out of date");
    }

    m_frameIndex = (m_frameIndex + 1) % MaxFramesInFlight;
}