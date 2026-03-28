#pragma once

#include "Context.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// A render graph resource
struct RenderResource
{
    std::string name;
    vk::Format format;
    vk::Extent2D extent;
    vk::ImageUsageFlags usage;
    vk::ImageLayout initialLayout;
    vk::ImageLayout finalLayout;
};

// A render pass and its draw operations
struct RenderPass
{
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::function<void(vk::raii::CommandBuffer &)> executeFunc;
};

/*
Renderflow:
 - geometry pass
 - lighting pass
 - post procesing
 - imgui

for each resourse
- format
- acces
- layout

 for each pass:
    - resources it uses
    - resource it writes tp
    - draw commands to buffer

each pass needs sync at some point

resources can be added and defines separately - all ok
resources are used by render passes - cool

therefore, for the current triangle
- One pass that
    - no inputs (nothing to read from)
    - one output (image for swapchain)

render graph needs to be told about the swapchain image to render to somehpow

so

-Swapchain: Get next image in chain to render to
- Tell RenderGraph this is the final imagfe to render to
    - what if the RenderGraph had the idea of

*/

class RenderGraph
{
   public:
    RenderGraph(Context &context);
    ~RenderGraph() = default;

    void AddResource(const RenderResource &resource);
    void AddPass(const RenderPass &pass);

    void Compile();
    void Render(vk::raii::CommandBuffer &cmdBuffer);

   private:
    Context &m_context;

    std::unordered_map<std::string, RenderResource> m_resources;
    std::vector<RenderPass> m_passes;

    std::vector<size_t> m_executionOrder;
};