#include "Swapchain.h"
#include "ImageFormat.h"
#include "LoggingLib/Logging.h"
#include <algorithm>
#include <ranges>

Swapchain::Swapchain(Context &context, GLFWwindow *window)
    : m_context(context), m_window(window)
{
    RecreateSwapchain();

    // Init render resources for each frame
    const vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = context.GraphicsIndex()};
    m_commandPool = vk::raii::CommandPool(context.GetDevice(), poolInfo);

    const vk::CommandBufferAllocateInfo allocInfo{
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

void Swapchain::Resize(const events::WindowResize & /*resize*/)
{
    RecreateSwapchain();
}

const vk::Format &Swapchain::GetFormat() const { return m_format; }

FrameInfo Swapchain::BeginFrame()
{
    const auto &fence = m_inFlightFences[m_frameIndex];
    const auto &presentCompleteSemaphore =
        m_presentCompleteSemaphores[m_frameIndex];

    const auto &device = m_context.GetDevice();
    const auto fenceResult =
        m_context.GetDevice().waitForFences(*fence, vk::True, UINT64_MAX);

    auto &cmdBuffer = m_commandBuffers[m_frameIndex];

    if (fenceResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to wait for fence!");
    }

    if (m_minimised)
    {
        // The window is minimised and has a size of zero. Cannot be rendered to
        return FrameInfo{.extent = m_swapExtent, .skip = true};
    }

    const auto [result, imageIndex] = m_swapChain.acquireNextImage(
        UINT64_MAX, *presentCompleteSemaphore, nullptr);
    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        // Swapchain needs recreating
        RecreateSwapchain();

        // Signal that we want to skip this frame
        return FrameInfo{.extent = m_swapExtent, .skip = true};
    }
    else if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to acquire next swapchain image");
    }

    // Reset fences as we now know we will be sumbitting work
    device.resetFences(*fence);
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

    const auto clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    const vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView = imageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor};

    const vk::RenderingInfo renderingInfo = {
        .renderArea = {.offset = {0, 0}, .extent = m_swapExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo};

    cmdBuffer.beginRendering(renderingInfo);

    return FrameInfo{.extent = m_swapExtent,
                     .image = image,
                     .imageView = imageView,
                     .commandBuffer = cmdBuffer};
}

void Swapchain::EndFrame()
{
    const auto &fence = m_inFlightFences[m_frameIndex];
    const auto &presentCompleteSemaphore =
        m_presentCompleteSemaphores[m_frameIndex];

    const auto &renderFinishedSemaphore =
        m_renderFinishedSemaphores[m_imageIndex];

    auto &cmdBuffer = m_commandBuffers[m_frameIndex];
    auto &image = m_swapChainImages[m_imageIndex];

    cmdBuffer.endRendering();

    renderer::vulkan::TransitionImageLayout(
        cmdBuffer, image, vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite, {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe);
    cmdBuffer.end();

    const vk::PipelineStageFlags waitDestinationStageMask{
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
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
    if (presentResult == vk::Result::eSuboptimalKHR ||
        presentResult == vk::Result::eErrorOutOfDateKHR)
    {
        LOG_WARN("Suboptimal KHR returned, swapchain out of date");
        RecreateSwapchain();
    }
    else if (presentResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to present swapchain image");
    }

    m_frameIndex = (m_frameIndex + 1) % MaxFramesInFlight;
}

void Swapchain::RecreateSwapchain()
{
    LOG_DEBUG("Creating swapchain");

    // Get the current size of the framebuffer
    int width{};
    int height{};
    glfwGetFramebufferSize(m_window, &width, &height);
    if (width == 0 || height == 0)
    {
        // Window is minimised, cannot have a swapchain of size zero
        m_minimised = true;
        return;
    }
    m_minimised = false;

    // Do not recreate swapchain until nothing is happening
    m_context.GetDevice().waitIdle();

    const auto swapchainInfo = m_context.GetSwapchainInfo();

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

    const vk::SwapchainCreateInfoKHR swapChainCreateInfo{
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
        // Current swapchain (if we have one)
        .oldSwapchain = m_swapChain != nullptr ? *m_swapChain : nullptr};

    // Create the swapchain. NOTE: As we cannot destroy the old swapchain until
    // we have created the new one, assign and them move
    auto swapchain =
        vk::raii::SwapchainKHR(m_context.GetDevice(), swapChainCreateInfo);
    m_swapChain = std::move(swapchain);

    m_swapChainImages = m_swapChain.getImages();

    // If we have previous image views, get rid of them
    m_swapChainImageViews.clear();
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
        m_swapChainImageViews.emplace_back(m_context.GetDevice(),
                                           imageViewCreateInfo);
    }
}