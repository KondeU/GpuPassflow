#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class ResourceBuffer {
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
    ResourceBuffer() = default;
    virtual ~ResourceBuffer() = default;
};

class ResourceBufferEx {
public:
    struct Description {
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
    ResourceBufferEx() = default;
    virtual ~ResourceBufferEx() = default;
};

}
