#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class UniformBuffer {
public:
    struct Description final {
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
    UniformBuffer() = default;
    virtual ~UniformBuffer() = default;
};

}
