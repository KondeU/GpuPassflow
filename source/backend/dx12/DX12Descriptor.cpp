#include "DX12Descriptor.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12Descriptor::DX12Descriptor(DX12Device& internal,
    DX12DescriptorHeap& heap, unsigned int index)
    : internal(internal), heap(heap)
    , indexInHeap(index)
{
    device = internal.NativeDevice();
    mResourceDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mImageSamplerDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    mRenderTargetViewDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDepthStencilViewDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

DX12Descriptor::~DX12Descriptor()
{
    Shutdown();
}

void DX12Descriptor::Setup(Description description)
{
    this->description = description;

    descriptorHandleIncrementSize = 0;
    if (gp::EnumCast(description.type) & gp::EnumCast(rhi::DescriptorType::ShaderResource)) {
        descriptorHandleIncrementSize = mResourceDescriptorHandleIncrementSize;
    } else if (gp::EnumCast(description.type) & gp::EnumCast(rhi::DescriptorType::ImageSampler)) {
        descriptorHandleIncrementSize = mImageSamplerDescriptorHandleIncrementSize;
    } else if (gp::EnumCast(description.type) & gp::EnumCast(rhi::DescriptorType::ColorOutput)) {
        descriptorHandleIncrementSize = mRenderTargetViewDescriptorHandleIncrementSize;
    } else if (gp::EnumCast(description.type) & gp::EnumCast(rhi::DescriptorType::DepthStencil)) {
        descriptorHandleIncrementSize = mDepthStencilViewDescriptorHandleIncrementSize;
    } else {
        GP_LOG_RET_E(TAG, "The descriptor type is invalid!");
    }

    if (ConvertDescriptorHeap(description.type) != heap.GetHeapType()) {
        GP_LOG_RET_E(TAG, "The descriptor type is not match with the heap type!");
    }

    auto hCpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap.Heap()->GetCPUDescriptorHandleForHeapStart());
    hCpu.Offset(indexInHeap, descriptorHandleIncrementSize);
    hCpuDescriptor = hCpu;

    auto hGpu = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap.Heap()->GetGPUDescriptorHandleForHeapStart());
    hGpu.Offset(indexInHeap, descriptorHandleIncrementSize);
    hGpuDescriptor = hGpu;
}

void DX12Descriptor::Shutdown()
{
    description = { rhi::DescriptorType::ConstantBuffer };
    hCpuDescriptor = {};
    hGpuDescriptor = {};
    pResource = static_cast<void*>(nullptr);
}

void DX12Descriptor::BuildDescriptor(rhi::ResourceBuffer* resource)
{
    if (heap.GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        GP_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support buffer!");
    }

    auto dxResource = dynamic_cast<DX12ResourceBuffer*>(resource);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = dxResource->Buffer()->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = dxResource->GetAllocatedBytesSize();

    device->CreateConstantBufferView(&cbvDesc, hCpuDescriptor);

    pResource = dxResource;
}

void DX12Descriptor::BuildDescriptor(rhi::ResourceBufferEx* resource, bool write)
{
    if (heap.GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        GP_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support buffer!");
    }

    auto dxResource = dynamic_cast<DX12ResourceBufferEx*>(resource);

    if (!write) {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = dxResource->GetElementsCount();
        srvDesc.Buffer.StructureByteStride = dxResource->GetElementBytesSize();
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        device->CreateShaderResourceView(dxResource->Buffer().Get(), &srvDesc, hCpuDescriptor);
    } else {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = dxResource->GetElementsCount();
        uavDesc.Buffer.StructureByteStride = dxResource->GetElementBytesSize();
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
        device->CreateUnorderedAccessView(dxResource->Buffer().Get(),
            NULL, // see: https://docs.microsoft.com/en-us/windows/win32/direct3d12/uav-counters
            &uavDesc, hCpuDescriptor);
    }

    pResource = dxResource;
}

void DX12Descriptor::BuildDescriptor(rhi::ResourceImage* resource, bool write)
{
    auto dxResource = dynamic_cast<DX12ResourceImage*>(resource);

    switch (heap.GetHeapType()) {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        if (!write) {
            device->CreateShaderResourceView(
                dxResource->Buffer().Get(), NULL, hCpuDescriptor);
        } else {
            device->CreateUnorderedAccessView(
                dxResource->Buffer().Get(), NULL, NULL, hCpuDescriptor);
        }
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        device->CreateRenderTargetView(dxResource->Buffer().Get(), NULL, hCpuDescriptor);
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        device->CreateDepthStencilView(dxResource->Buffer().Get(), NULL, hCpuDescriptor);
        break;
    default:
        GP_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support image!");
    }

    pResource = dxResource;
}

void DX12Descriptor::BuildDescriptor(rhi::ImageSampler* sampler)
{
    if (heap.GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        GP_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support sampler!");
    }

    auto dxSampler = dynamic_cast<DX12ImageSampler*>(sampler);

    device->CreateSampler(&dxSampler->NativeSamplerState(), hCpuDescriptor);

    pResource = dxSampler;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Descriptor::AttachmentView() const
{
    return hCpuDescriptor;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Descriptor::NativeCpuDescriptor() const
{
    return hCpuDescriptor;
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12Descriptor::NativeGpuDescriptor() const
{
    return hGpuDescriptor;
}

bool DX12Descriptor::IsNativeDescriptorsContinuous(
    const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& descriptors)
{
    if (descriptors.size() > 0) {
        for (size_t n = 0; n < descriptors.size(); n++) {
            CD3DX12_CPU_DESCRIPTOR_HANDLE current(hCpuDescriptor);
            current.Offset(static_cast<INT>(n), descriptorHandleIncrementSize);
            if (current != descriptors[n]) {
                return false;
            }
        }
        return true;
    }
    return false; // descriptors is empty!
}

DX12ResourceBuffer* DX12Descriptor::BindedResourceBuffer() const
{
    auto ptr = std::get_if<DX12ResourceBuffer*>(&pResource);
    if (!ptr) {
        GP_LOG_RETN_E(TAG, "Get binded resource buffer failed, this descriptor is not build with "
            "ResourceBuffer, or maybe you forgot to call the BuildDescriptor function.");
    }
    return *ptr;
}

DX12ResourceBufferEx* DX12Descriptor::BindedResourceBufferEx() const
{
    auto ptr = std::get_if<DX12ResourceBufferEx*>(&pResource);
    if (!ptr) {
        GP_LOG_RETN_E(TAG, "Get binded resource buffer failed, this descriptor is not build with "
            "ResourceBufferEx, or maybe you forgot to call the BuildDescriptor function.");
    }
    return *ptr;
}

DX12ResourceImage* DX12Descriptor::BindedResourceImage() const
{
    auto ptr = std::get_if<DX12ResourceImage*>(&pResource);
    if (!ptr) {
        GP_LOG_RETN_E(TAG, "Get binded resource buffer failed, this descriptor is not build with "
            "ResourceImage, or maybe you forgot to call the BuildDescriptor function.");
    }
    return *ptr;
}

DX12ImageSampler* DX12Descriptor::BindedImageSampler() const
{
    auto ptr = std::get_if<DX12ImageSampler*>(&pResource);
    if (!ptr) {
        GP_LOG_RETN_E(TAG, "Get binded resource buffer failed, this descriptor is not build with "
            "ImageSampler, or maybe you forgot to call the BuildDescriptor function.");
    }
    return *ptr;
}

}
