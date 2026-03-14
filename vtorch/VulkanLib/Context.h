#pragma once
#include "VulkanIncluder.h"

class Context
{
   public:
    Context();

   private:
    vk::raii::Context m_context;
    vk::raii::Instance m_instance{nullptr};
};