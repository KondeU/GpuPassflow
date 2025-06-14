#pragma once

#include "VulkanBackendHeaders.h"
#include "backend/BasicTypes.h"

namespace au::backend {

VkFormat ConvertBasicFormat(rhi::BasicFormat format);

VkFormat ConvertIndexFormat(rhi::IndexFormat format);

VkFormat ConvertVertexFormat(rhi::VertexFormat format);

VkVertexInputRate ConvertInputClassification(rhi::VertexInputRate rate);

VkPrimitiveTopology ConvertPrimitiveTopology(rhi::PrimitiveTopology topology);

VkMemoryPropertyFlags ConvertHeap(rhi::TransferDirection type);

VkImageLayout ConvertImageLayout(rhi::ResourceState state);

VkAccessFlags ConvertAccessFlags(rhi::ResourceState state);

VkPipelineStageFlags ConvertPipelineStageFlags(rhi::ResourceState state);

VkImageUsageFlags ConvertImageUsageFlags(rhi::ImageType type);

VkImageType ConvertImageType(rhi::ImageDimension dimension);

VkImageViewType ConvertImageViewType(rhi::ImageDimension dimension);

VkDescriptorType ConvertDescriptorType(rhi::DescriptorType type);

VkShaderStageFlags ConvertShaderStage(rhi::ShaderStage stage);

VkClearValue ConvertClearValue(rhi::BasicFormat format, const rhi::ClearValue& value);

VkPolygonMode ConvertFillMode(rhi::FillMode mode);

VkCullModeFlags ConvertCullMode(rhi::CullMode mode);

VkSamplerAddressMode ConvertAddressMode(rhi::AddressMode mode);

VkFilter ConvertFilter(rhi::SamplerState::Filter filter);

VkSamplerMipmapMode ConvertMipmapMode(rhi::SamplerState::Filter filter);

VkAttachmentLoadOp ConvertLoadOp(rhi::PassAction action);

VkAttachmentStoreOp ConvertStoreOp(rhi::PassAction action);

} 