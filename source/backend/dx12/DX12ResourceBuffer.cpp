#include "DX12ResourceBuffer.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12ResourceBuffer::DX12ResourceBuffer(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceBuffer::~DX12ResourceBuffer()
{
    Shutdown();
}

void DX12ResourceBuffer::Setup(Description description)
{
    this->description = description;

    allocatedBytesSize = CalculateAlignedBytesSize(description.bufferBytesSize);
    if (allocatedBytesSize == 0) {
        GP_LOG_RET_F(TAG, "Create constant buffer failed, buffer size is zero!");
    }

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(allocatedBytesSize),
        ConvertResourceState(rhi::ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));
}

void DX12ResourceBuffer::Shutdown()
{
    description = { 0 };
    allocatedBytesSize = 0;
    buffer.Reset();
}

void* DX12ResourceBuffer::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12ResourceBuffer::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

unsigned int DX12ResourceBuffer::GetBufferBytesSize() const
{
    return description.bufferBytesSize;
}

unsigned int DX12ResourceBuffer::GetAllocatedBytesSize() const
{
    return allocatedBytesSize;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceBuffer::Buffer()
{
    return buffer;
}

unsigned int DX12ResourceBuffer::CalculateAlignedBytesSize(unsigned int input)
{
    // Constant buffers must be a multiple of the minimum hardware allocation size,
    // it usually 256 bytes. So round up to nearest multiple of 256. We do this by
    // adding 255 and then masking off the lower two bytes which store all bits < 256.
    // Example: Suppose bytes size = 300, then:
    //   (300 + 255) & ~255
    //   555 & ~255
    //   0x022B & ~0x..000FF
    //   0x022B &  0x..FFF00
    //   0x0200
    //   512
    return (input + 255u) & ~255u;
}

DX12ResourceBufferEx::DX12ResourceBufferEx(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceBufferEx::~DX12ResourceBufferEx()
{
    Shutdown();
}

void DX12ResourceBufferEx::Setup(Description description)
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

void DX12ResourceBufferEx::Shutdown()
{
    description = { 0, 0 };
    buffer.Reset();
}

void* DX12ResourceBufferEx::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12ResourceBufferEx::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

unsigned int DX12ResourceBufferEx::GetElementsCount() const
{
    return description.elementsCount;
}

unsigned int DX12ResourceBufferEx::GetElementBytesSize() const
{
    return description.elementBytesSize;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceBufferEx::Buffer()
{
    return buffer;
}

}
