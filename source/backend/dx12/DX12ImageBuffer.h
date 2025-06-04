#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12Device;

class DX12ImageBuffer : public rhi::ImageBuffer
    , DX12Object<DX12ImageBuffer> {
public:
    explicit DX12ImageBuffer(DX12Device& device);
    ~DX12ImageBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    D3D12_CLEAR_VALUE RenderTargetClearValue() const;
    D3D12_CLEAR_VALUE DepthStencilClearValue() const;
    D3D12_CLEAR_FLAGS DepthStencilClearFlags() const;

    unsigned int GetRowBytesSize() const;
    unsigned int GetSliceBytesSize() const;
    unsigned int GetTotalBytesSize() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ rhi::BasicFormat::R32G32B32A32_FLOAT, 0u, 0u };
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
