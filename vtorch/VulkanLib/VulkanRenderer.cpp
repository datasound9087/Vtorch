#include "VulkanRenderer.h"
#include "LoggingLib/Logging.h"

namespace
{
    GraphicsPipeline CreateGraphicsPipeline(Context &context,
                                            Swapchain &swapchain)
    {
        LOG_DEBUG("Creatng pipeline");
        GraphicsPipelineBuilder builder{context};
        builder.Shader("C:/Users/Sam/dev/Vtorch/bazel-out/x64_windows-"
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

VulkanRenderer::VulkanRenderer(GLFWwindow *window)
    : m_context(window), m_swapchain(m_context, window),
      m_graphicsPipeline(CreateGraphicsPipeline(m_context, m_swapchain))
{
}
void VulkanRenderer::Init() {}