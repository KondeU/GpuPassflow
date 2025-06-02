#include "DX12IndexAttribute.h"
#include "DX12BasicTypes.h"

namespace au::backend {

DX12IndexAttribute::DX12IndexAttribute()
{
}

DX12IndexAttribute::~DX12IndexAttribute()
{
    Shutdown();
}

void DX12IndexAttribute::Setup(Description description)
{
    if (description.reserved != 1) {
        description.reserved = 1;
    }
    informations.resize(description.reserved, { DXGI_FORMAT_UNKNOWN });
}

void DX12IndexAttribute::Shutdown()
{
    informations.resize(0, { DXGI_FORMAT_UNKNOWN });
}

void DX12IndexAttribute::SetAttribute(Attribute attribute)
{
    informations.back() = {
        ConvertIndexFormat(attribute.format),
        ConvertPrimitiveTopology(attribute.topology),
        ConvertPrimitiveTopologyType(attribute.topology),
        ConvertStripValue(attribute.stripValue)
    };
}

const DX12IndexAttribute::IndexInformation&
DX12IndexAttribute::GetIndexInformation() const
{
    return informations.back();
}

}
