#include "DX12InputVertex.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12InputVertex::DX12InputVertex(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12InputVertex::~DX12InputVertex()
{
    Shutdown();
}

void DX12InputVertex::Setup(Description description)
{
    this->description = description;

    bufferTotalByteSize = description.verticesCount * description.attributesByteSize;
    if (bufferTotalByteSize == 0) {
        GP_LOG_RET_F(TAG, "Create vertex buffer failed, buffer size is zero!");
    }

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferTotalByteSize),
        ConvertResourceState(rhi::ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));
}

void DX12InputVertex::Shutdown()
{
    description = { 0u, 0u };
    bufferTotalByteSize = 0u;
    buffer.Reset();
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12InputVertex::Buffer()
{
    return buffer;
}

D3D12_VERTEX_BUFFER_VIEW DX12InputVertex::BufferView(DX12InputVertexAttributes* attributes) const
{
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    vbv.BufferLocation = buffer->GetGPUVirtualAddress();
    vbv.SizeInBytes = bufferTotalByteSize;
    vbv.StrideInBytes = description.attributesByteSize;
    return vbv;
}

void* DX12InputVertex::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12InputVertex::Unmap()
{
    if (description.memoryType != rhi::TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

}
