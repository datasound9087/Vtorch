#pragma once
#include "VulkanIncluder.h"
#include <GLFW/glfw3.h>

class Context
{
   public:
    Context(GLFWwindow *window);

   private:
    vk::raii::Context m_context;
    vk::raii::Instance m_instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger{nullptr};
    vk::raii::SurfaceKHR m_surface{nullptr};

    vk::raii::PhysicalDevice m_physicalDevice{nullptr};
    vk::raii::Device m_device{nullptr};

    vk::raii::Queue m_graphicsQueue{nullptr};
};