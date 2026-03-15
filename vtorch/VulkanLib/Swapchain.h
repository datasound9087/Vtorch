#pragma once
#include "Context.h"
#include "VulkanIncluder.h"
#include <GLFW/glfw3.h>
#include <vector>

struct FrameInfo
{
    vk::raii::CommandBuffer *commandBuffer;
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
    uint32_t m_imageIndex{};

    // Resources for each swapchain frame
    vk::raii::CommandPool m_commandPool{nullptr};
    vk::raii::CommandBuffers m_commandBuffers{nullptr};
    std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence> m_inFlightFences;
    uint32_t m_frameIndex{};
};