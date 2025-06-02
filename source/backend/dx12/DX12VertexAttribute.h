#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12VertexAttribute : public rhi::VertexAttribute
    , DX12Object<DX12VertexAttribute> {
public:
    explicit DX12VertexAttribute();
    ~DX12VertexAttribute() override;

    void Setup(Description description);
    void Shutdown();

    void AddAttribute(Attribute attribute) override;
    void ClearAttributes() override;

    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElements() const;

private:
    std::vector<std::string> semantics;
    std::vector<D3D12_INPUT_ELEMENT_DESC> elements;
};

}
