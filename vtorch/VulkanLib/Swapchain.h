#pragma once
#include "Context.h"
#include "VulkanIncluder.h"
#include <GLFW/glfw3.h>
#include <vector>

class Swapchain
{
   public:
    Swapchain(Context &context, GLFWwindow *window);

   private:
    vk::raii::SwapchainKHR m_swapChain{nullptr};
    std::vector<vk::Image> m_swapChainImages;
};