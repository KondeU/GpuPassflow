#pragma once

#include "DX12BackendHeaders.h"

namespace au::backend {

DXGI_FORMAT ConvertBasicFormat(BasicFormat format);

DXGI_FORMAT ConvertIndexFormat(IndexFormat format);

DXGI_FORMAT ConvertVertexFormat(VertexFormat format);

D3D12_INPUT_CLASSIFICATION ConvertInputClassification(VertexInputRate rate);

D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ConvertStripValue(IndexStripCutValue value);

D3D12_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(PrimitiveTopology topology);

D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(PrimitiveTopology topology);

D3D12_HEAP_TYPE ConvertHeap(TransferDirection type);

D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state);

unsigned int ConvertMSAA(MSAA msaa);

D3D12_RESOURCE_FLAGS ConvertImageResourceFlag(ImageType type);

D3D12_RESOURCE_DIMENSION ConvertImageDimension(ImageDimension dimension);

D3D12_DESCRIPTOR_HEAP_TYPE ConvertDescriptorHeap(DescriptorType type);

D3D12_DESCRIPTOR_HEAP_FLAGS ConvertDescriptorHeapVisible(DescriptorType type);

D3D12_DESCRIPTOR_RANGE_TYPE ConvertDescriptorRangeType(DescriptorType type, bool& success);

D3D12_SHADER_VISIBILITY ConvertShaderVisibility(ShaderStage visibility);

D3D12_CLEAR_VALUE ConvertClearValue(BasicFormat format, ClearValue value);

D3D12_CLEAR_FLAGS ConvertClearFlags(BasicFormat format);

D3D12_FILL_MODE ConvertFillMode(FillMode mode);

D3D12_CULL_MODE ConvertCullMode(CullMode mode);

D3D12_RASTERIZER_DESC ConvertRasterizerState(RasterizerState state);

D3D12_TEXTURE_ADDRESS_MODE ConvertAddressMode(AddressMode mode);

D3D12_SAMPLER_DESC ConvertSamplerState(SamplerState state);

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE ConvertRenderPassBeginningAccessType(PassAction action);

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE ConvertRenderPassEndingAccessType(PassAction action);

}
