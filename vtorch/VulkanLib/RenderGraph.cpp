#include "RenderGraph.h"

RenderGraph::RenderGraph(Context &context) : m_context(context) {}

void RenderGraph::AddResource(const std::string & /*name*/,
                              const RenderResource & /*resource*/)
{
}

void RenderGraph::AddPass(const RenderPass & /*pass*/) {}

void RenderGraph::Compile() {}

void RenderGraph::Render(vk::CommandBuffer & /*cmdBuffer*/) {}
