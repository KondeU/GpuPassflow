#include "DX12InputIndex.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12InputIndex::DX12InputIndex(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12InputIndex::~DX12InputIndex()
{
    Shutdown();
}

void DX12InputIndex::Setup(Description description)
{
    this->description = description;

    bufferTotalByteSize = description.indicesCount * description.indexByteSize;
    if (bufferTotalByteSize == 0) {
        AU_LOG_RET_F(TAG, "Create index buffer failed, buffer size is zero!");
    }

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferTotalByteSize),
        ConvertResourceState(ResourceState::GENERAL_READ),
        NULL, IID_PPV_ARGS(&buffer)));
}

void DX12InputIndex::Shutdown()
{
    description = { 0u, 0u };
    bufferTotalByteSize = 0u;
    buffer.Reset();
}

void* DX12InputIndex::Map()
{
    void* mapped = nullptr;
    if (description.memoryType != TransferDirection::GPU_ONLY) {
        LogIfFailedF(buffer->Map(0, NULL, &mapped));
    }
    return mapped;
}

void DX12InputIndex::Unmap()
{
    if (description.memoryType != TransferDirection::GPU_ONLY) {
        buffer->Unmap(0, NULL);
    }
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12InputIndex::Buffer()
{
    return buffer;
}

D3D12_INDEX_BUFFER_VIEW DX12InputIndex::BufferView(DX12InputIndexAttribute* attribute) const
{
    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = buffer->GetGPUVirtualAddress();
    ibv.SizeInBytes = bufferTotalByteSize;
    ibv.Format = attribute->GetIndexInformation().IndexFormat;
    return ibv;
}

UINT DX12InputIndex::IndicesCount() const
{
    return description.indicesCount;
}

}
