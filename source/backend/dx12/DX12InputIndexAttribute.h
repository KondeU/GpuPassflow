#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12InputIndexAttribute : public InputIndexAttribute
    , DX12Object<DX12InputIndexAttribute> {
public:
    explicit DX12InputIndexAttribute();
    ~DX12InputIndexAttribute() override;

    void Setup(Description description);
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
