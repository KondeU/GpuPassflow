#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class IndexAttribute {
public:
    struct Description final {
        unsigned int reserved = 1;
    };

    struct Attribute final {
        IndexFormat format;
        PrimitiveTopology topology;
        IndexStripCutValue stripValue;

        Attribute(
            IndexFormat format,
            PrimitiveTopology topology,
            IndexStripCutValue stripValue = IndexStripCutValue::NONE_OR_DISABLE)
            : format(format)
            , topology(topology)
            , stripValue(stripValue)
        {}
    };

    virtual void SetAttribute(Attribute attribute) = 0;

protected:
    IndexAttribute() = default;
    virtual ~IndexAttribute() = default;
};

}
