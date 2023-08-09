#pragma once

#include "DX12Descriptor.h"

namespace au::backend {

class DX12Device;

class DX12DescriptorHeap : public rhi::DescriptorHeap
    , DX12Object<DX12DescriptorHeap> {
public:
    explicit DX12DescriptorHeap(DX12Device& device);
    ~DX12DescriptorHeap() override;

    void Setup(Description description);
    void Shutdown();

    Descriptor* AllocateDescriptor(Descriptor::Description description) override;

    D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, DescriptorType::ShaderResource };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;

    std::vector<std::unique_ptr<DX12Descriptor>> descriptors;
};

}
