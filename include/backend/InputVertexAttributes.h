#pragma once

#include <string>
#include "BasicTypes.h"

namespace au::rhi {

class InputVertexAttributes {
public:
    struct Description {
        unsigned int reserved = 8; // Unused yet
    };

    struct Attribute {
        VertexFormat format;
        std::string semantic;
        unsigned int location;
        unsigned int stride;
        unsigned int slot;
        VertexInputRate slotClass;

        Attribute(
            VertexFormat format,
            std::string semantic,
            unsigned int location,
            unsigned int stride,
            unsigned int slot = 0,
            VertexInputRate slotClass = VertexInputRate::PER_VERTEX)
            : format(format)
            , semantic(semantic)
            , location(location)
            , stride(stride)
            , slot(slot)
            , slotClass(slotClass)
        {}
    };

    virtual void AddAttribute(Attribute attribute) = 0;
    virtual void ClearAttributes() = 0;

protected:
    InputVertexAttributes() = default;
    virtual ~InputVertexAttributes() = default;
};

}
