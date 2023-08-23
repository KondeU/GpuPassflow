#pragma once

#include "DX12BackendHeaders.h"

namespace au::backend {

DXGI_FORMAT ConvertBasicFormat(rhi::BasicFormat format);

DXGI_FORMAT ConvertIndexFormat(rhi::IndexFormat format);

DXGI_FORMAT ConvertVertexFormat(rhi::VertexFormat format);

D3D12_INPUT_CLASSIFICATION ConvertInputClassification(rhi::VertexInputRate rate);

D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ConvertStripValue(rhi::IndexStripCutValue value);

D3D12_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(rhi::PrimitiveTopology topology);

D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(rhi::PrimitiveTopology topology);

D3D12_HEAP_TYPE ConvertHeap(rhi::TransferDirection type);

D3D12_RESOURCE_STATES ConvertResourceState(rhi::ResourceState state);

unsigned int ConvertMSAA(rhi::MSAA msaa);

D3D12_RESOURCE_FLAGS ConvertImageResourceFlag(rhi::ImageType type);

D3D12_RESOURCE_DIMENSION ConvertImageDimension(rhi::ImageDimension dimension);

D3D12_DESCRIPTOR_HEAP_TYPE ConvertDescriptorHeap(rhi::DescriptorType type);

D3D12_DESCRIPTOR_HEAP_FLAGS ConvertDescriptorHeapVisible(rhi::DescriptorType type);

D3D12_DESCRIPTOR_RANGE_TYPE ConvertDescriptorRangeType(rhi::DescriptorType type, bool& success);

D3D12_SHADER_VISIBILITY ConvertShaderVisibility(rhi::ShaderStage visibility);

D3D12_CLEAR_VALUE ConvertClearValue(rhi::BasicFormat format, rhi::ClearValue value);

D3D12_CLEAR_FLAGS ConvertClearFlags(rhi::BasicFormat format);

D3D12_FILL_MODE ConvertFillMode(rhi::FillMode mode);

D3D12_CULL_MODE ConvertCullMode(rhi::CullMode mode);

D3D12_RASTERIZER_DESC ConvertRasterizerState(rhi::RasterizerState state);

D3D12_TEXTURE_ADDRESS_MODE ConvertAddressMode(rhi::AddressMode mode);

D3D12_SAMPLER_DESC ConvertSamplerState(rhi::SamplerState state);

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE ConvertRenderPassBeginAccessType(rhi::PassAction action);

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE ConvertRenderPassEndAccessType(rhi::PassAction action);

}
