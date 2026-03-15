#pragma once
#include "Context.h"
#include "RendererLib/IRenderer.h"
#include "Swapchain.h"

#include <GLFW/glfw3.h>

class VulkanRenderer : public IRenderer
{
   public:
    VulkanRenderer(GLFWwindow *window);
    void Init() override;

   private:
    Context m_context;
    Swapchain m_swapchain;
};