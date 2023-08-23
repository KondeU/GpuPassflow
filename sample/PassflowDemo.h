#pragma once

#include "GpuPassflow.h"
#include "directxmath/DirectXMath.h"

class PassflowDemo {
public:
    virtual void Setup() = 0;
    virtual void ExecuteOneFrame() = 0;
    virtual void SizeChanged(void* window, unsigned int width, unsigned int height) = 0;
    virtual ~PassflowDemo() = default;
};
