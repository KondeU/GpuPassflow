#include "passflow/pass/resource/PassProperties.h"

namespace au::gp {

rhi::VertexAttribute::Attribute InputProperties::MakeDefaultPositionVertexAttribute()
{
    return { rhi::VertexFormat::FLOAT32x3, "POSITION", 0, 0 };
}

}
