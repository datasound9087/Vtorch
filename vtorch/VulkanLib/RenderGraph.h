#pragma once

#include "Context.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// A render graph resource
struct RenderResource
{
    vk::Format format;
    vk::Extent2D extent;
    vk::ImageUsageFlags usage;
    vk::ImageLayout initialLayout;
    vk::ImageLayout finalLayout;

    vk::Image image;
    vk::ImageView imageView;
};

// A render pass and its draw operations
struct RenderPass
{
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::function<void(vk::raii::CommandBuffer &)> renderFunc;
};

class RenderGraph
{
   public:
    static inline const std::string Swapchain{"swapchain"};

    RenderGraph(Context &context);
    ~RenderGraph() = default;

    void AddResource(const std::string &name, const RenderResource &resource);
    void AddPass(const RenderPass &pass);

    void Compile();
    void Render(vk::CommandBuffer &cmdBuffer);

   private:
    Context &m_context;

    std::unordered_map<std::string, RenderResource> m_resources;
    std::vector<RenderPass> m_passes;

    std::vector<size_t> m_executionOrder;
};