#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer(GLFWwindow *window)
    : m_context(window), m_swapchain(m_context, window)
{
}
void VulkanRenderer::Init() {}