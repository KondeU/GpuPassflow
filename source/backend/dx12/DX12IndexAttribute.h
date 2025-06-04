#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12IndexAttribute : public rhi::IndexAttribute
    , DX12Object<DX12IndexAttribute> {
public:
    explicit DX12IndexAttribute();
    ~DX12IndexAttribute() override;

    bool Setup(Description description);
    void Shutdown();

    void SetAttribute(Attribute attribute) override;

    struct IndexInformation {
        DXGI_FORMAT IndexFormat;
        D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE StripCutValue;

        IndexInformation(DXGI_FORMAT IndexFormat,
            D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology
                = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType
                = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE StripCutValue
                = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED)
            : IndexFormat(IndexFormat)
            , PrimitiveTopology(PrimitiveTopology)
            , PrimitiveTopologyType(PrimitiveTopologyType)
            , StripCutValue(StripCutValue)
        {}
    };

    const IndexInformation& GetIndexInformation() const;

private:
    std::vector<IndexInformation> informations;
};

}
