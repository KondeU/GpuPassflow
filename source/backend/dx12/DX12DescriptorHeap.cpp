#include "DX12DescriptorHeap.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12DescriptorHeap::DX12DescriptorHeap(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12DescriptorHeap::~DX12DescriptorHeap()
{
    Shutdown();
}

void DX12DescriptorHeap::Setup(Description description)
{
    this->description = description;

    if (description.capacity == 0) {
        AU_LOG_RET_F(TAG, "Create descriptor heap failed, capacity is zero!");
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = description.capacity;
    heapDesc.Type = ConvertDescriptorHeap(description.type);
    heapDesc.Flags = ConvertDescriptorHeapVisible(description.type);
    heapDesc.NodeMask = 0;
    LogIfFailedF(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
}

void DX12DescriptorHeap::Shutdown()
{
    description = { 0u, DescriptorType::ShaderResource };
    heap.Reset();
    descriptors.resize(0);
}

Descriptor* DX12DescriptorHeap::AllocateDescriptor(Descriptor::Description description)
{
    unsigned int index = static_cast<unsigned int>(descriptors.size());
    return CreateInstance<Descriptor>(descriptors, description, internal, *this, index);
}

D3D12_DESCRIPTOR_HEAP_TYPE DX12DescriptorHeap::GetHeapType() const
{
    return ConvertDescriptorHeap(description.type);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DX12DescriptorHeap::Heap()
{
    return heap;
}

}
