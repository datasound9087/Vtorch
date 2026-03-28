#include "VulkanRenderer.h"
#include "LoggingLib/Logging.h"
#include <gsl/gsl>

namespace
{
    GraphicsPipeline CreateGraphicsPipeline(Context &context,
                                            Swapchain &swapchain)
    {
        LOG_DEBUG("Creatng pipeline");
        GraphicsPipelineBuilder builder{context};
        builder.Shader("D:/dev/Vtorch/bazel-out/x64_windows-"
                       "fastbuild/bin/shaders/triangle.spirv",
                       vk::ShaderStageFlagBits::eVertex |
                           vk::ShaderStageFlagBits::eFragment);
        builder.VertexInput({});
        builder.AssemblyState(
            {.topology = vk::PrimitiveTopology::eTriangleList});
        builder.RasterizerState({.depthClampEnable = vk::False,
                                 .rasterizerDiscardEnable = vk::False,
                                 .polygonMode = vk::PolygonMode::eFill,
                                 .cullMode = vk::CullModeFlagBits::eBack,
                                 .frontFace = vk::FrontFace::eClockwise,
                                 .depthBiasEnable = vk::False,
                                 .depthBiasSlopeFactor = 1.0f,
                                 .lineWidth = 1.0f});
        builder.MultiSampling(
            {.rasterizationSamples = vk::SampleCountFlagBits::e1,
             .sampleShadingEnable = vk::False});

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable = vk::False,
            .colorWriteMask = vk::ColorComponentFlagBits::eR |
                              vk::ColorComponentFlagBits::eG |
                              vk::ColorComponentFlagBits::eB |
                              vk::ColorComponentFlagBits::eA};
        builder.ColorBlend({.logicOpEnable = vk::False,
                            .logicOp = vk::LogicOp::eCopy,
                            .attachmentCount = 1,
                            .pAttachments = &colorBlendAttachment});
        builder.DepthStencil({});
        builder.Attachments(
            {.colorAttachmentCount = 1,
             .pColorAttachmentFormats = &swapchain.GetFormat()});
        return builder.Build();
    }
} // namespace

VulkanRenderer::VulkanRenderer(GLFWwindow *window, event::SystemBus &systemBus)
    : m_context(window), m_swapchain(m_context, window),
      m_renderGraph(m_context),
      m_graphicsPipeline(CreateGraphicsPipeline(m_context, m_swapchain))
{
    // Handle swapchain resize
    m_resizeEventSub = systemBus.subscribe<events::WindowResize>(
        [&](const auto &resize) { m_swapchain.Resize(resize); });
}

VulkanRenderer::~VulkanRenderer() { m_context.GetDevice().waitIdle(); }

void VulkanRenderer::Init() {}

void VulkanRenderer::RenderFrame()
{
    auto frame = m_swapchain.BeginFrame();
    // If the swapchain has resized, skip the frame as it cannot be rendered to
    if (frame.skip)
    {
        return;
    }

    const auto endFrame = gsl::finally([&]() { m_swapchain.EndFrame(); });

    // Add swapchain image to render to
    m_renderGraph.AddResource(
        RenderGraph::Swapchain,
        {.format = frame.format,
         .extent = frame.extent,
         .initialLayout = vk::ImageLayout::eColorAttachmentOptimal,
         .finalLayout = vk::ImageLayout::ePresentSrcKHR,
         .image = frame.image,
         .imageView = frame.imageView});

    m_renderGraph.Compile();
    m_renderGraph.Render(frame.commandBuffer);
}