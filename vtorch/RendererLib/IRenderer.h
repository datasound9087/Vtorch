#pragma once
#include <memory>

class IRenderer
{
   public:
    virtual ~IRenderer() = default;
    virtual void Init() = 0;
};