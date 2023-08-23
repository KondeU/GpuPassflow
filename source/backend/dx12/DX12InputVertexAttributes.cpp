#include "DX12InputVertexAttributes.h"
#include "DX12BasicTypes.h"

namespace au::backend {

DX12InputVertexAttributes::DX12InputVertexAttributes()
{
}

DX12InputVertexAttributes::~DX12InputVertexAttributes()
{
    Shutdown();
}

void DX12InputVertexAttributes::Setup(Description description)
{
    semantics.reserve(description.reserved);
    elements.reserve(description.reserved);
}

void DX12InputVertexAttributes::Shutdown()
{
    semantics.resize(0);
    elements.resize(0);
}

void DX12InputVertexAttributes::AddAttribute(Attribute attribute)
{
    // NB: Make sure the semantic name is unique and the slot+location is unique.
    semantics.emplace_back(attribute.semantic);
    D3D12_INPUT_ELEMENT_DESC elementDesc{};
    elementDesc.SemanticName = semantics.back().c_str();
    elementDesc.SemanticIndex = 0;
    elementDesc.Format = ConvertVertexFormat(attribute.format);
    elementDesc.InputSlot = attribute.slot;
    elementDesc.InputSlotClass = ConvertInputClassification(attribute.slotClass);
    elementDesc.AlignedByteOffset = attribute.stride;
    elementDesc.InstanceDataStepRate = 0;
    elements.emplace_back(elementDesc);
    // NB: The change of the semantics names container will invalidate the result of c_str()!
    for (size_t n = 0; n < semantics.size(); n++) {
        elements[n].SemanticName = semantics[n].c_str();
    }
}

void DX12InputVertexAttributes::ClearAttributes()
{
    semantics.clear();
    elements.clear();
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& DX12InputVertexAttributes::GetInputElements() const
{
    return elements;
}

}
