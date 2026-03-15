#pragma once
#include "Context.h"
#include <filesystem>
#include <optional>
#include <vector>

struct ShaderFile
{
    const std::vector<char> code;
    vk::Flags<vk::ShaderStageFlagBits> shaderStages;
    vk::raii::ShaderModule module{nullptr};
};

struct GraphicsPipeline
{
    vk::raii::PipelineLayout layout{nullptr};
    vk::raii::Pipeline pipeline{nullptr};
};

class GraphicsPipelineBuilder
{
   public:
    GraphicsPipelineBuilder(Context &context);

    GraphicsPipelineBuilder &
    Shader(const std::filesystem::path &path,
           const vk::Flags<vk::ShaderStageFlagBits> shaderStages);

    GraphicsPipelineBuilder &
    VertexInput(const vk::PipelineVertexInputStateCreateInfo &vertexInputState);

    GraphicsPipelineBuilder &AssemblyState(
        const vk::PipelineInputAssemblyStateCreateInfo &assemblyState);

    GraphicsPipelineBuilder &RasterizerState(
        const vk::PipelineRasterizationStateCreateInfo &rasterState);

    GraphicsPipelineBuilder &MultiSampling(
        const vk::PipelineMultisampleStateCreateInfo &multiSampleState);

    GraphicsPipelineBuilder &DepthStencil(
        const vk::PipelineDepthStencilStateCreateInfo &depthStencilState);

    GraphicsPipelineBuilder &
    ColorBlend(const vk::PipelineColorBlendStateCreateInfo &colorBlendState);

    GraphicsPipelineBuilder &
    Attachments(const vk::PipelineRenderingCreateInfo &renderingCreateInfo);

    GraphicsPipeline Build();

   private:
    Context &m_context;
    std::vector<ShaderFile> m_shaderFiles;

    std::optional<vk::PipelineVertexInputStateCreateInfo> m_vertexInputState;
    std::optional<vk::PipelineInputAssemblyStateCreateInfo> m_assemblyState;
    std::optional<vk::PipelineRasterizationStateCreateInfo> m_rasterizerState;
    std::optional<vk::PipelineMultisampleStateCreateInfo> m_multisamplingState;
    std::optional<vk::PipelineDepthStencilStateCreateInfo> m_depthStencilState;
    std::optional<vk::PipelineColorBlendStateCreateInfo> m_colorBlendState;
    std::optional<vk::PipelineRenderingCreateInfo> m_renderingInfo;
};