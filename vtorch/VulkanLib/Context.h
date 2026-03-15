#pragma once
#include "VulkanIncluder.h"
#include <GLFW/glfw3.h>
#include <vector>

struct SwapchainInfo
{
    vk::SurfaceKHR surface;
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class Context
{
   public:
    Context(GLFWwindow *window);

    const SwapchainInfo GetSwapchainInfo() const;
    const vk::raii::Device &GetDevice() const;

   private:
    vk::raii::Context m_context;
    vk::raii::Instance m_instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger{nullptr};
    vk::raii::SurfaceKHR m_surface{nullptr};

    vk::raii::PhysicalDevice m_physicalDevice{nullptr};
    vk::raii::Device m_device{nullptr};

    vk::raii::Queue m_graphicsQueue{nullptr};
};