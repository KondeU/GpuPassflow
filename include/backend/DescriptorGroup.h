#pragma once

#include <utility>
#include "BasicTypes.h"

namespace au::backend {

class DescriptorGroup {
public:
    struct Description {
        unsigned int space;

        Description(
            unsigned int space)
            : space(space)
        {}
    };

    virtual void AddDescriptor(DescriptorType type,
        unsigned int id, ShaderStage visibility) = 0;

    virtual void AddDescriptors(DescriptorType type,
        std::pair<unsigned int, unsigned int> range, ShaderStage visibility) = 0;

protected:
    DescriptorGroup() = default;
    virtual ~DescriptorGroup() = default;
};

}
