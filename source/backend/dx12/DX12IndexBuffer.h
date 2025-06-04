#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12Device;
class DX12IndexAttribute;

class DX12IndexBuffer : public rhi::IndexBuffer
    , DX12Object<DX12IndexBuffer> {
public:
    explicit DX12IndexBuffer(DX12Device& device);
    ~DX12IndexBuffer() override;

    bool Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    D3D12_INDEX_BUFFER_VIEW BufferView(DX12IndexAttribute* attribute) const;

    UINT IndicesCount() const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, 0u };
    unsigned int bufferTotalByteSize = 0u;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
