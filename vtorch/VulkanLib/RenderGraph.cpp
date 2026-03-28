#include "RenderGraph.h"

RenderGraph::RenderGraph(Context &context) {}

void RenderGraph::AddResource(const RenderResource &resource) {}

void RenderGraph::AddPass(const RenderPass &pass) {}

void RenderGraph::Compile() {}

void RenderGraph::Render(vk::raii::CommandBuffer &cmdBuffer) {}
