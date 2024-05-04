#include "DX12ResourceStorageBuffer.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12ResourceStorageBuffer::DX12ResourceStorageBuffer(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceStorageBuffer::~DX12ResourceStorageBuffer()
{
    Shutdown();
}

void DX12ResourceStorageBuffer::Setup(Description description)
{
    this->description = description;

    UINT64 allocatedBytesSize = static_cast<UINT64>(description.elementsCount)
                              * static_cast<UINT64>(description.elementBytesSize);

    D3D12_RESOURCE_FLAGS bufferResourceFlag = description.writableResourceInShader ?
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)), D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(allocatedBytesSize, bufferResourceFlag),
        ConvertResourceState(rhi::ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));
}

void DX12ResourceStorageBuffer::Shutdown()
{
    description = { 0, 0 };
    buffer.Reset();
}

void* DX12ResourceStorageBuffer::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12ResourceStorageBuffer::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

unsigned int DX12ResourceStorageBuffer::GetElementsCount() const
{
    return description.elementsCount;
}

unsigned int DX12ResourceStorageBuffer::GetElementBytesSize() const
{
    return description.elementBytesSize;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceStorageBuffer::Buffer()
{
    return buffer;
}

}
