#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;
class DX12InputVertexAttributes;

class DX12InputVertex : public InputVertex
    , DX12Object<DX12InputVertex> {
public:
    explicit DX12InputVertex(DX12Device& device);
    ~DX12InputVertex() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    D3D12_VERTEX_BUFFER_VIEW BufferView(DX12InputVertexAttributes* attributes) const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, 0u };
    unsigned int bufferTotalByteSize = 0u;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
