#include "DX12InputIndexAttribute.h"
#include "DX12BasicTypes.h"

namespace au::backend {

DX12InputIndexAttribute::DX12InputIndexAttribute()
{
}

DX12InputIndexAttribute::~DX12InputIndexAttribute()
{
    Shutdown();
}

void DX12InputIndexAttribute::Setup(Description description)
{
    informations.resize(1, { DXGI_FORMAT_UNKNOWN });
}

void DX12InputIndexAttribute::Shutdown()
{
    informations.resize(0, { DXGI_FORMAT_UNKNOWN });
}

void DX12InputIndexAttribute::SetAttribute(Attribute attribute)
{
    informations.back() = {
        ConvertIndexFormat(attribute.format),
        ConvertPrimitiveTopology(attribute.topology),
        ConvertPrimitiveTopologyType(attribute.topology),
        ConvertStripValue(attribute.stripValue)
    };
}

const DX12InputIndexAttribute::IndexInformation&
DX12InputIndexAttribute::GetIndexInformation() const
{
    return informations.back();
}

}
