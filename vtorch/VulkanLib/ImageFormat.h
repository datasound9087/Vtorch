#pragma once
#include "VulkanIncluder.h"

namespace renderer::vulkan
{
    void TransitionImageLayout(vk::raii::CommandBuffer &cmdBuffer,
                               vk::Image &image, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout,
                               vk::AccessFlags2 srcAccess,
                               vk::AccessFlags2 dstAccess,
                               vk::PipelineStageFlags2 srcStage,
                               vk::PipelineStageFlags2 dstStage);
};