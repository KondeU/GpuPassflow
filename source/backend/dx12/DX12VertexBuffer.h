#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12Device;
class DX12VertexAttribute;

class DX12VertexBuffer : public rhi::VertexBuffer
    , DX12Object<DX12VertexBuffer> {
public:
    explicit DX12VertexBuffer(DX12Device& device);
    ~DX12VertexBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    D3D12_VERTEX_BUFFER_VIEW BufferView(DX12VertexAttribute* attributes) const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, 0u };
    unsigned int bufferTotalByteSize = 0u;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
