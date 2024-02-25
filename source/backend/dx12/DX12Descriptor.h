#pragma once

#include <variant>
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;
class DX12DescriptorHeap;
class DX12ResourceConstantBuffer;
class DX12ResourceStorageBuffer;
class DX12ResourceImage;
class DX12ImageSampler;

class DX12Descriptor : public rhi::Descriptor
    , DX12Object<DX12Descriptor> {
public:
    explicit DX12Descriptor(DX12Device& device, DX12DescriptorHeap& heap, unsigned int index);
    ~DX12Descriptor() override;

    void Setup(Description description);
    void Shutdown();

    void BuildDescriptor(rhi::ResourceConstantBuffer* resource) override;
    void BuildDescriptor(rhi::ResourceStorageBuffer* resource, bool write) override;
    void BuildDescriptor(rhi::ResourceImage* resource, bool write) override;
    void BuildDescriptor(rhi::ImageSampler* sampler) override;

    D3D12_CPU_DESCRIPTOR_HANDLE AttachmentView() const;      // RTV/DSV
    D3D12_CPU_DESCRIPTOR_HANDLE NativeCpuDescriptor() const; // CBV/SRV/UAV/Sampler
    D3D12_GPU_DESCRIPTOR_HANDLE NativeGpuDescriptor() const; // CBV/SRV/UAV/Sampler
    bool IsNativeDescriptorsContinuous(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

    DX12ResourceConstantBuffer* BindedResourceConstantBuffer() const;
    DX12ResourceStorageBuffer* BindedResourceStorageBuffer() const;
    DX12ResourceImage* BindedResourceImage() const;
    DX12ImageSampler* BindedImageSampler() const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    DX12DescriptorHeap& heap;
    unsigned int indexInHeap = 0;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT mResourceDescriptorHandleIncrementSize = 0;         // CBV/SRV/UAV
    UINT mImageSamplerDescriptorHandleIncrementSize = 0;     // ImageSampler
    UINT mRenderTargetViewDescriptorHandleIncrementSize = 0; // RTV
    UINT mDepthStencilViewDescriptorHandleIncrementSize = 0; // DSV

    Description description{ rhi::DescriptorType::ConstantBuffer };
    UINT descriptorHandleIncrementSize = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor;

    std::variant<void*,
        DX12ResourceConstantBuffer*,
        DX12ResourceStorageBuffer*,
        DX12ResourceImage*,
        DX12ImageSampler*
    > pResource;
};

}
