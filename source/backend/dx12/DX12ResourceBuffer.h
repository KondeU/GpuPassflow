#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;

class DX12ResourceBuffer : public rhi::ResourceBuffer
    , DX12Object<DX12ResourceBuffer> {
public:
    explicit DX12ResourceBuffer(DX12Device& device);
    ~DX12ResourceBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    unsigned int GetBufferBytesSize() const;
    unsigned int GetAllocatedBytesSize() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

protected:
    static unsigned int CalculateAlignedBytesSize(unsigned int input);

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0 };
    unsigned int allocatedBytesSize = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

class DX12ResourceBufferEx : public rhi::ResourceBufferEx
    , DX12Object<DX12ResourceBufferEx> {
public:
    explicit DX12ResourceBufferEx(DX12Device& device);
    ~DX12ResourceBufferEx() override;

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
