#pragma once

#include "BasicTypes.h"

namespace au::rhi {

// Storage buffer object (SBO) in Vulkan
// Structured buffer object in DX12
class ResourceStorageBuffer {
public:
    struct Description final {
        unsigned int elementsCount;
        unsigned int elementBytesSize;
        bool writableResourceInShader;
        TransferDirection memoryType;

        Description(
            unsigned int elementsCount,
            unsigned int elementBytesSize,
            bool writableResourceInShader = false,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : elementsCount(elementsCount)
            , elementBytesSize(elementBytesSize)
            , writableResourceInShader(writableResourceInShader)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    ResourceStorageBuffer() = default;
    virtual ~ResourceStorageBuffer() = default;
};

}
