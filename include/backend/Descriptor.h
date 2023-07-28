#pragma once

#include "BasicTypes.h"

namespace au::backend {

class ResourceBuffer;
class ResourceImage;
class ImageSampler;
class ResourceBufferEx;
class ResourceImageEx;

class Descriptor {
public:
    struct Description {
        DescriptorType type;

        Description(
            DescriptorType type)
            : type(type)
        {}
    };

    virtual void BuildDescriptor(ResourceBuffer* resource) = 0;
    virtual void BuildDescriptor(ResourceBufferEx* resource, bool write) = 0;
    virtual void BuildDescriptor(ResourceImage* resource, bool write) = 0;
    virtual void BuildDescriptor(ImageSampler* sampler) = 0;

protected:
    Descriptor() = default;
    virtual ~Descriptor() = default;
};

}
