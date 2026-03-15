#include "Swapchain.h"
#include "LoggingLib/Logging.h"
#include <algorithm>
#include <ranges>

Swapchain::Swapchain(Context &context, GLFWwindow *window)
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

    const vk::Extent2D swapExtent{
        .width = std::clamp<uint32_t>(
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
        .imageExtent = swapExtent,
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
}