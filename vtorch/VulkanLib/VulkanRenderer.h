#pragma once
#include "Context.h"
#include "EventLib/EventBus.h"
#include "GraphicsPipelineBuilder.h"
#include "RendererLib/IRenderer.h"
#include "Swapchain.h"

#include <GLFW/glfw3.h>

class VulkanRenderer : public IRenderer
{
   public:
    VulkanRenderer(GLFWwindow *window, event::SystemBus &systemBus);
    ~VulkanRenderer();
    void Init() override;
    void RenderFrame() override;

   private:
    Context m_context;
    Swapchain m_swapchain;
    GraphicsPipeline m_graphicsPipeline;

    // Event subscriptions are RAII based, so must keep in scope.
    event::Subscription m_resizeEventSub;
};
