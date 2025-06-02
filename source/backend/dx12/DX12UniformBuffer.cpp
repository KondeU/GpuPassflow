#include "DX12UniformBuffer.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12UniformBuffer::DX12UniformBuffer(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12UniformBuffer::~DX12UniformBuffer()
{
    Shutdown();
}

void DX12UniformBuffer::Setup(Description description)
{
    this->description = description;

    allocatedBytesSize = CalculateAlignedBytesSize(description.bufferBytesSize);
    if (allocatedBytesSize == 0) {
        GP_LOG_F(TAG, "Create constant buffer failed, buffer size is zero!");
        return;
    }

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(allocatedBytesSize),
        ConvertResourceState(rhi::ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));
}

void DX12UniformBuffer::Shutdown()
{
    description = { 0 };
    allocatedBytesSize = 0;
    buffer.Reset();
}

void* DX12UniformBuffer::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12UniformBuffer::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

unsigned int DX12UniformBuffer::GetBufferBytesSize() const
{
    return description.bufferBytesSize;
}

unsigned int DX12UniformBuffer::GetAllocatedBytesSize() const
{
    return allocatedBytesSize;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12UniformBuffer::Buffer()
{
    return buffer;
}

unsigned int DX12UniformBuffer::CalculateAlignedBytesSize(unsigned int input)
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
