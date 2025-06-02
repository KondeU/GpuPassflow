#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12Device;

class DX12ResourceStorageBuffer : public rhi::StorageBuffer
    , DX12Object<DX12ResourceStorageBuffer> {
public:
    explicit DX12ResourceStorageBuffer(DX12Device& device);
    ~DX12ResourceStorageBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    unsigned int GetElementsCount() const;
    unsigned int GetElementBytesSize() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0, 0 };
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
