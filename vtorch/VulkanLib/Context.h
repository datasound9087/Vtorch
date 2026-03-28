#pragma once
#include "VulkanAllocator.h"
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
    uint32_t GraphicsIndex() const;
    const vk::raii::Queue &GraphicsQueue() const;
    const VulkanAllocator &GetAllocator() const;

   private:
    vk::raii::Context m_context;
    vk::raii::Instance m_instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger{nullptr};
    vk::raii::SurfaceKHR m_surface{nullptr};

    vk::raii::PhysicalDevice m_physicalDevice{nullptr};
    uint32_t m_graphicsIndex{};
    vk::raii::Device m_device{nullptr};

    vk::raii::Queue m_graphicsQueue{nullptr};

    VulkanAllocator m_vmaAllocator{nullptr};
};