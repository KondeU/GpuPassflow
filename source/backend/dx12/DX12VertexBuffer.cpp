#include "DX12VertexBuffer.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12VertexBuffer::DX12VertexBuffer(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12VertexBuffer::~DX12VertexBuffer()
{
    Shutdown();
}

bool DX12VertexBuffer::Setup(Description description)
{
    this->description = description;

    bufferTotalByteSize = description.verticesCount * description.attributesByteSize;
    if (bufferTotalByteSize == 0) {
        GP_LOG_F(TAG, "Create vertex buffer failed, buffer size is zero!");
        return false;
    }

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferTotalByteSize),
        ConvertResourceState(rhi::ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));
    return true;
}

void DX12VertexBuffer::Shutdown()
{
    description = { 0u, 0u };
    bufferTotalByteSize = 0u;
    buffer.Reset();
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12VertexBuffer::Buffer()
{
    return buffer;
}

D3D12_VERTEX_BUFFER_VIEW DX12VertexBuffer::BufferView(DX12VertexAttribute* attributes) const
{
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    vbv.BufferLocation = buffer->GetGPUVirtualAddress();
    vbv.SizeInBytes = bufferTotalByteSize;
    vbv.StrideInBytes = description.attributesByteSize;
    return vbv;
}

void* DX12VertexBuffer::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12VertexBuffer::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

}
