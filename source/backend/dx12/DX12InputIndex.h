#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;
class DX12InputIndexAttribute;

class DX12InputIndex : public InputIndex
    , DX12Object<DX12InputIndex> {
public:
    explicit DX12InputIndex(DX12Device& device);
    ~DX12InputIndex() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    D3D12_INDEX_BUFFER_VIEW BufferView(DX12InputIndexAttribute* attribute) const;

    UINT IndicesCount() const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, 0u };
    unsigned int bufferTotalByteSize = 0u;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
