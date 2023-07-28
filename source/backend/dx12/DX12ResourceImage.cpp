#include "DX12ResourceImage.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12ResourceImage::DX12ResourceImage(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12ResourceImage::~DX12ResourceImage()
{
    Shutdown();
}

void DX12ResourceImage::Setup(Description description)
{
    this->description = description;

    D3D12_RESOURCE_DESC resourceDesc{};
    if (description.memoryType == TransferDirection::GPU_ONLY) {
        resourceDesc.Dimension = ConvertImageDimension(description.dimension);
        resourceDesc.Alignment = 0;
        resourceDesc.Width = description.width;
        resourceDesc.Height = description.height;
        resourceDesc.DepthOrArraySize = description.arrays;
        resourceDesc.MipLevels = description.mips;
        resourceDesc.Format = ConvertBasicFormat(description.format);
        resourceDesc.SampleDesc.Count = ConvertMSAA(description.msaa);
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = ConvertImageResourceFlag(description.usage);
        if ((description.writableResourceInShader)/* &&
            (description.usage == ImageType::ShaderResource)*/) {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
    } else {
        // Used as a buffer for upload or readback.
        UINT64 bytes = static_cast<UINT64>(QueryBasicFormatBytes(description.format)) *
            description.width * description.height *
            description.arrays * description.mips; // TODO: MSAA
        resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bytes);
    }
    D3D12_CLEAR_VALUE clearValue = ConvertClearValue(description.format, description.clearValue);

    LogIfFailedF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(ConvertHeap(description.memoryType)),
        D3D12_HEAP_FLAG_NONE, &resourceDesc,
        ConvertResourceState(ResourceState::GENERAL_READ),
        ((description.usage == ImageType::ShaderResource) ?
            NULL : &clearValue), IID_PPV_ARGS(&buffer)));
}

void DX12ResourceImage::Shutdown()
{
    description = { BasicFormat::R32G32B32A32_FLOAT, 0u, 0u };
    buffer.Reset();
}

void* DX12ResourceImage::Map(unsigned int msaaLayer)
{
    void* mapped = nullptr;
    if (description.memoryType != TransferDirection::GPU_ONLY
        && msaaLayer < ConvertMSAA(description.msaa)) {
        LogIfFailedF(buffer->Map(msaaLayer, NULL, &mapped));
    }
    return mapped;
}

void DX12ResourceImage::Unmap(unsigned int msaaLayer)
{
    if (description.memoryType != TransferDirection::GPU_ONLY
        && msaaLayer < ConvertMSAA(description.msaa)) {
        buffer->Unmap(msaaLayer, NULL);
    }
}

D3D12_CLEAR_VALUE DX12ResourceImage::RenderTargetClearValue() const
{
    return ConvertClearValue(description.format, description.clearValue);
}

D3D12_CLEAR_VALUE DX12ResourceImage::DepthStencilClearValue() const
{
    return ConvertClearValue(description.format, description.clearValue);
}

D3D12_CLEAR_FLAGS DX12ResourceImage::DepthStencilClearFlags() const
{
    return ConvertClearFlags(description.format);
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12ResourceImage::Buffer()
{
    return buffer;
}

}
