#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;

class DX12ResourceImage : public rhi::ResourceImage
    , DX12Object<DX12ResourceImage> {
public:
    explicit DX12ResourceImage(DX12Device& device);
    ~DX12ResourceImage() override;

    void Setup(Description description);
    void Shutdown();

    void* Map(unsigned int msaaLayer) override;
    void Unmap(unsigned int msaaLayer) override;

    D3D12_CLEAR_VALUE RenderTargetClearValue() const;
    D3D12_CLEAR_VALUE DepthStencilClearValue() const;
    D3D12_CLEAR_FLAGS DepthStencilClearFlags() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ rhi::BasicFormat::R32G32B32A32_FLOAT, 0u, 0u };
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
