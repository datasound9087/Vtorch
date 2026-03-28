#include "VulkanAllocator.h"

VulkanAllocator::VulkanAllocator(const VmaAllocatorCreateInfo &createInfo)
{
    vmaCreateAllocator(&createInfo, &m_allocator);
}

VulkanAllocator::~VulkanAllocator() { vmaDestroyAllocator(m_allocator); }

VulkanAllocator::VulkanAllocator(VulkanAllocator &&other) noexcept
{
    std::exchange(other.m_allocator, {});
}

VulkanAllocator &VulkanAllocator::operator=(VulkanAllocator &&rhs) noexcept
{
    std::swap(m_allocator, rhs.m_allocator);
    return *this;
}