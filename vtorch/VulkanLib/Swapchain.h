#pragma once
#include "Context.h"
#include "VulkanIncluder.h"
#include <GLFW/glfw3.h>
#include <vector>

struct FrameInfo
{
    vk::raii::CommandBuffer &commandBuffer;
    vk::Extent2D &frameExtent;
};

class Swapchain
{
   public:
    static const int MaxFramesInFlight = 2;

   public:
    Swapchain(Context &context, GLFWwindow *window);

    const vk::Format &GetFormat() const;

    FrameInfo BeginFrame();
    void EndFrame();

   private:
    Context &m_context;
    vk::Format m_format;
    vk::Extent2D m_swapExtent;
    vk::raii::SwapchainKHR m_swapChain{nullptr};
    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::raii::ImageView> m_swapChainImageViews;

    // Resources for each swapchain frame
    vk::raii::CommandPool m_commandPool{nullptr};
    vk::raii::CommandBuffers m_commandBuffers{nullptr};
    std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence> m_inFlightFences;

    // The image currently being rendered to (m_swapChainImages and
    // m_swapChainImageViews)
    uint32_t m_imageIndex{};
    // The current frame being rendered to. NOTE: This is different to
    // m_imageIndex, as the image acquired from the swapchain for the frame may
    // be different to this.
    uint32_t m_frameIndex{};
};