#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class IndexBuffer {
public:
    struct Description final {
        unsigned int indicesCount;
        unsigned int indexByteSize;
        TransferDirection memoryType;

        Description(
            unsigned int indicesCount,
            unsigned int indexByteSize,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : indicesCount(indicesCount)
            , indexByteSize(indexByteSize)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;
};

}
