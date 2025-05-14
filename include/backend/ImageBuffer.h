#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class ImageBuffer {
public:
    struct Description final {
        BasicFormat format;
        uint32_t width;  // 1st dimension
        uint32_t height; // 2nd dimension
        uint8_t arrays;  // 3rd dimension
        uint8_t mips;    // mipmap
        ImageType usage;
        ImageDimension dimension;
        ClearValue clearValue;
        TransferDirection memoryType;
        bool writableResourceInShader;

        Description(
            BasicFormat format,
            uint32_t width,
            uint32_t height,
            uint8_t arrays = 1,
            uint8_t mips = 1,
            ClearValue clearValue = {},
            ImageType usage = ImageType::ShaderResource,
            ImageDimension dimension = ImageDimension::Dimension2D,
            TransferDirection memoryType = TransferDirection::GPU_ONLY,
            bool writableResourceInShader = false)
            : format(format)
            , width(width)
            , height(height)
            , arrays(arrays)
            , mips(mips)
            , usage(usage)
            , dimension(dimension)
            , clearValue(clearValue)
            , memoryType(memoryType)
            , writableResourceInShader(writableResourceInShader)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    ImageBuffer() = default;
    virtual ~ImageBuffer() = default;
};

}
