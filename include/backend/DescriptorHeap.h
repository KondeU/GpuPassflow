#pragma once

#include "Descriptor.h"

namespace au::backend {

class DescriptorHeap {
public:
    struct Description {
        unsigned int capacity; // max descriptor count
        DescriptorType type;   // descriptor heap type

        // A draw call in a pass can only set up one heap of each type
        //  at a time, so reserve enough descriptor capacity as possible.
        Description(
            unsigned int capacity,
            DescriptorType heapType)
            : capacity(capacity)
            , type(heapType)
        {}
    };

    virtual Descriptor* AllocateDescriptor(Descriptor::Description description) = 0;

protected:
    DescriptorHeap() = default;
    virtual ~DescriptorHeap() = default;
};

}
