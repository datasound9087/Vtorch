#pragma once

#include "VulkanIncluder.h"
#include <vk_mem_alloc.h>

class VulkanAllocator
{
   public:
    VulkanAllocator(std::nullptr_t) {}
    VulkanAllocator(const VmaAllocatorCreateInfo &createInfo);
    ~VulkanAllocator();

    VulkanAllocator(const VulkanAllocator &other) = delete;
    VulkanAllocator(VulkanAllocator &&other) noexcept;
    VulkanAllocator &operator=(const VulkanAllocator &rhs) = delete;
    VulkanAllocator &operator=(VulkanAllocator &&rhs) noexcept;

   private:
    VmaAllocator m_allocator;
};