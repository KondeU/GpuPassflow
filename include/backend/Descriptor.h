#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class ResourceConstantBuffer;
class ResourceStorageBuffer;
class ResourceImage;
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

    virtual void BuildDescriptor(ResourceConstantBuffer* resource) = 0;
    virtual void BuildDescriptor(ResourceStorageBuffer* resource, bool write) = 0;
    virtual void BuildDescriptor(ResourceImage* resource, bool write) = 0;
    virtual void BuildDescriptor(ImageSampler* sampler) = 0;

protected:
    Descriptor() = default;
    virtual ~Descriptor() = default;
};

}
