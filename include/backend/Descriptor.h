#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class UniformBuffer;
class StorageBuffer;
class ImageBuffer;
class ImageSampler;

class Descriptor {
public:
    struct Description {
        DescriptorType type;

        Description(
            DescriptorType type)
            : type(type)
        {}
    };

    virtual void BuildDescriptor(UniformBuffer* resource) = 0;
    virtual void BuildDescriptor(StorageBuffer* resource, bool write) = 0;
    virtual void BuildDescriptor(ImageBuffer* resource, bool write) = 0;
    virtual void BuildDescriptor(ImageSampler* sampler) = 0;

protected:
    Descriptor() = default;
    virtual ~Descriptor() = default;
};

}
