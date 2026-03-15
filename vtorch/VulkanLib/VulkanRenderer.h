#pragma once
#include "Context.h"
#include "GraphicsPipelineBuilder.h"
#include "RendererLib/IRenderer.h"
#include "Swapchain.h"

#include <GLFW/glfw3.h>

class VulkanRenderer : public IRenderer
{
   public:
    VulkanRenderer(GLFWwindow *window);
    ~VulkanRenderer();
    void Init() override;
    void RenderFrame() override;

   private:
    Context m_context;
    Swapchain m_swapchain;
    GraphicsPipeline m_graphicsPipeline;
};