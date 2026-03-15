#pragma once
#include "Context.h"
#include "VulkanIncluder.h"
#include <GLFW/glfw3.h>
#include <vector>

class Swapchain
{
   public:
    Swapchain(Context &context, GLFWwindow *window);

    const vk::Format &GetFormat() const;

   private:
    vk::Format m_format;
    vk::raii::SwapchainKHR m_swapChain{nullptr};
    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::raii::ImageView> m_swapChainImageViews;
};