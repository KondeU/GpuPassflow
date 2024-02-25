#pragma once

#include "BasicTypes.h"

namespace au::rhi {

// Uniform buffer object (UBO) in Vulkan
// Constant buffer object (CBO) in DX12
class ResourceConstantBuffer {
public:
    struct Description {
        unsigned int bufferBytesSize;
        TransferDirection memoryType;

        Description(
            unsigned int bufferBytesSize,
            TransferDirection memoryType = TransferDirection::CPU_TO_GPU)
            : bufferBytesSize(bufferBytesSize)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    ResourceConstantBuffer() = default;
    virtual ~ResourceConstantBuffer() = default;
};

}
