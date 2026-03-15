#include "ImageFormat.h"

namespace renderer::vulkan
{
    void TransitionImageLayout(vk::raii::CommandBuffer &cmdBuffer,
                               vk::Image &image, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout,
                               vk::AccessFlags2 srcAccess,
                               vk::AccessFlags2 dstAccess,
                               vk::PipelineStageFlags2 srcStage,
                               vk::PipelineStageFlags2 dstStage)
    {
        vk::ImageMemoryBarrier2 barrier = {
            .srcStageMask = srcStage,
            .srcAccessMask = srcAccess,
            .dstStageMask = dstStage,
            .dstAccessMask = dstAccess,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                 .baseMipLevel = 0,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1}};
        vk::DependencyInfo dependency_info = {.dependencyFlags = {},
                                              .imageMemoryBarrierCount = 1,
                                              .pImageMemoryBarriers = &barrier};
        cmdBuffer.pipelineBarrier2(dependency_info);
    }
}; // namespace renderer::vulkan