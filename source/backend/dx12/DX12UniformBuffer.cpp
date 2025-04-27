#include "DX12ResourceConstantBuffer.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12ResourceConstantBuffer::DX12ResourceConstantBuffer(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceConstantBuffer::~DX12ResourceConstantBuffer()
{
    Shutdown();
}

void DX12ResourceConstantBuffer::Setup(Description description)
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

void DX12ResourceConstantBuffer::Shutdown()
{
    description = { 0 };
    allocatedBytesSize = 0;
    buffer.Reset();
}

void* DX12ResourceConstantBuffer::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12ResourceConstantBuffer::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

unsigned int DX12ResourceConstantBuffer::GetBufferBytesSize() const
{
    return description.bufferBytesSize;
}

unsigned int DX12ResourceConstantBuffer::GetAllocatedBytesSize() const
{
    return allocatedBytesSize;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceConstantBuffer::Buffer()
{
    return buffer;
}

unsigned int DX12ResourceConstantBuffer::CalculateAlignedBytesSize(unsigned int input)
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

}
