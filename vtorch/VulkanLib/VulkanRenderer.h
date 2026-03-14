#pragma once
#include "Context.h"
#include "RendererLib/IRenderer.h"

class VulkanRenderer : public IRenderer
{
   public:
    VulkanRenderer();
    void Init() override;

   private:
    Context m_context;
};