#include "GraphicsPipelineBuilder.h"
#include <fstream>

namespace
{
    std::vector<char> ReadBinaryFile(const std::filesystem::path &path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        std::vector<char> buffer{};
        buffer.resize(static_cast<size_t>(file.tellg()));

        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

        return buffer;
    }

    const char *EntryPointFor(const vk::ShaderStageFlagBits flagBits)
    {
        if (flagBits & vk::ShaderStageFlagBits::eVertex)
        {
            return "VertMain";
        };

        if (flagBits & vk::ShaderStageFlagBits::eFragment)
        {
            return "FragMain";
        };

        if (flagBits & vk::ShaderStageFlagBits::eCompute)
        {
            return "ComputeMain";
        }

        return "";
    }
} // namespace

GraphicsPipelineBuilder::GraphicsPipelineBuilder(Context &context)
    : m_context(context)
{
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::Shader(
    const std::filesystem::path &path,
    const vk::Flags<vk::ShaderStageFlagBits> shaderStages)
{
    m_shaderFiles.push_back(
        {.code = ReadBinaryFile(path), .shaderStages = shaderStages});

    auto &file = m_shaderFiles.back();
    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = file.code.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t *>(file.code.data())};
    file.module = vk::raii::ShaderModule(m_context.GetDevice(), createInfo);

    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::VertexInput(
    const vk::PipelineVertexInputStateCreateInfo &vertexInputState)
{
    m_vertexInputState = vertexInputState;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::AssemblyState(
    const vk::PipelineInputAssemblyStateCreateInfo &assemblyState)
{
    m_assemblyState = assemblyState;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::RasterizerState(
    const vk::PipelineRasterizationStateCreateInfo &rasterState)
{
    m_rasterizerState = rasterState;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::MultiSampling(
    const vk::PipelineMultisampleStateCreateInfo &multiSampleState)
{
    m_multisamplingState = multiSampleState;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::DepthStencil(
    const vk::PipelineDepthStencilStateCreateInfo &depthStencilState)
{
    m_depthStencilState = depthStencilState;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::ColorBlend(
    const vk::PipelineColorBlendStateCreateInfo &colorBlendState)
{
    m_colorBlendState = colorBlendState;
    return *this;
}

GraphicsPipelineBuilder &GraphicsPipelineBuilder::Attachments(
    const vk::PipelineRenderingCreateInfo &renderingCreateInfo)
{
    m_renderingInfo = renderingCreateInfo;
    return *this;
}

GraphicsPipeline GraphicsPipelineBuilder::Build()
{
    // Dynamic states
    std::vector dynamicStates = {vk::DynamicState::eViewport,
                                 vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()};

    // Shader stages
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStagesCreateInfos{};

    if (m_shaderFiles.empty())
    {
        throw std::runtime_error(
            "Pipeline creation failed: No shaders specified");
    }

    for (const auto &shaderFile : m_shaderFiles)
    {
        // A shader file contains multiple extry points for multiple shader
        // types, map to each type
        if (shaderFile.shaderStages & vk::ShaderStageFlagBits::eVertex)
        {
            shaderStagesCreateInfos.push_back(
                {.stage = vk::ShaderStageFlagBits::eVertex,
                 .module = shaderFile.module,
                 .pName = EntryPointFor(vk::ShaderStageFlagBits::eVertex)});
        }

        if (shaderFile.shaderStages & vk::ShaderStageFlagBits::eFragment)
        {
            shaderStagesCreateInfos.push_back(
                {.stage = vk::ShaderStageFlagBits::eFragment,
                 .module = shaderFile.module,
                 .pName = EntryPointFor(vk::ShaderStageFlagBits::eFragment)});
        }

        if (shaderFile.shaderStages & vk::ShaderStageFlagBits::eCompute)
        {
            shaderStagesCreateInfos.push_back(
                {.stage = vk::ShaderStageFlagBits::eCompute,
                 .module = shaderFile.module,
                 .pName = EntryPointFor(vk::ShaderStageFlagBits::eCompute)});
        }
    }

    GraphicsPipeline graphicsPipeline{};
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 0, .pushConstantRangeCount = 0};

    graphicsPipeline.layout =
        vk::raii::PipelineLayout(m_context.GetDevice(), pipelineLayoutInfo);

    // Empty for dynamic rendering
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{
        .viewportCount = 1,
        .pViewports = {},
        .scissorCount = 1,
        .pScissors = {}};

    if (!m_vertexInputState || !m_assemblyState || !m_rasterizerState ||
        !m_multisamplingState || !m_depthStencilState || !m_colorBlendState ||
        !m_renderingInfo)
    {
        throw std::runtime_error("Failed to create graphics pipeline: some "
                                 "required fields are null");
    }

    vk::GraphicsPipelineCreateInfo pipelineInfo{
        .pNext = &(*m_renderingInfo),
        .stageCount = static_cast<uint32_t>(shaderStagesCreateInfos.size()),
        .pStages = shaderStagesCreateInfos.data(),
        .pVertexInputState = &(*m_vertexInputState),
        .pInputAssemblyState = &(*m_assemblyState),
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &(*m_rasterizerState),
        .pMultisampleState = &(*m_multisamplingState),
        .pColorBlendState = &(*m_colorBlendState),
        .pDynamicState = &dynamicState,
        .layout = graphicsPipeline.layout,
        .renderPass = nullptr};

    graphicsPipeline.pipeline =
        vk::raii::Pipeline(m_context.GetDevice(), nullptr, pipelineInfo);

    return graphicsPipeline;
}