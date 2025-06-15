#include "VulkanBasicTypes.h"

#if defined(DEBUG) || defined(_DEBUG)
#define CONVERT_UNSUPPORTED(...) throw std::logic_error("bad conversion from rhi to vk!")
#else
#define CONVERT_UNSUPPORTED(...) GP_LOG_W(TAG, __VA_ARGS__)
#endif

namespace au::backend {

VkFormat ConvertBasicFormat(rhi::BasicFormat format)
{
    switch (format) {
    case rhi::BasicFormat::R32G32B32A32_FLOAT:  return VK_FORMAT_R32G32B32A32_SFLOAT;
    case rhi::BasicFormat::R32G32B32A32_UINT:   return VK_FORMAT_R32G32B32A32_UINT;
    case rhi::BasicFormat::R32G32B32A32_SINT:   return VK_FORMAT_R32G32B32A32_SINT;
    case rhi::BasicFormat::R32G32B32_FLOAT:     return VK_FORMAT_R32G32B32_SFLOAT;
    case rhi::BasicFormat::R32G32B32_UINT:      return VK_FORMAT_R32G32B32_UINT;
    case rhi::BasicFormat::R32G32B32_SINT:      return VK_FORMAT_R32G32B32_SINT;
    case rhi::BasicFormat::R16G16B16A16_FLOAT:  return VK_FORMAT_R16G16B16A16_SFLOAT;
    case rhi::BasicFormat::R16G16B16A16_UINT:   return VK_FORMAT_R16G16B16A16_UINT;
    case rhi::BasicFormat::R16G16B16A16_SINT:   return VK_FORMAT_R16G16B16A16_SINT;
    case rhi::BasicFormat::R16G16B16A16_UNORM:  return VK_FORMAT_R16G16B16A16_UNORM;
    case rhi::BasicFormat::R16G16B16A16_SNORM:  return VK_FORMAT_R16G16B16A16_SNORM;
    case rhi::BasicFormat::R32G32_FLOAT:        return VK_FORMAT_R32G32_SFLOAT;
    case rhi::BasicFormat::R32G32_UINT:         return VK_FORMAT_R32G32_UINT;
    case rhi::BasicFormat::R32G32_SINT:         return VK_FORMAT_R32G32_SINT;
    case rhi::BasicFormat::R10G10B10A2_UINT:    return VK_FORMAT_A2B10G10R10_UINT_PACK32;
    case rhi::BasicFormat::R10G10B10A2_UNORM:   return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case rhi::BasicFormat::R8G8B8A8_UINT:       return VK_FORMAT_R8G8B8A8_UINT;
    case rhi::BasicFormat::R8G8B8A8_SINT:       return VK_FORMAT_R8G8B8A8_SINT;
    case rhi::BasicFormat::R8G8B8A8_UNORM:      return VK_FORMAT_R8G8B8A8_UNORM;
    case rhi::BasicFormat::R8G8B8A8_UNORM_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
    case rhi::BasicFormat::R8G8B8A8_SNORM:      return VK_FORMAT_R8G8B8A8_SNORM;
    case rhi::BasicFormat::R16G16_FLOAT:        return VK_FORMAT_R16G16_SFLOAT;
    case rhi::BasicFormat::R16G16_UINT:         return VK_FORMAT_R16G16_UINT;
    case rhi::BasicFormat::R16G16_SINT:         return VK_FORMAT_R16G16_SINT;
    case rhi::BasicFormat::R16G16_UNORM:        return VK_FORMAT_R16G16_UNORM;
    case rhi::BasicFormat::R16G16_SNORM:        return VK_FORMAT_R16G16_SNORM;
    case rhi::BasicFormat::R32_FLOAT:           return VK_FORMAT_R32_SFLOAT;
    case rhi::BasicFormat::R32_UINT:            return VK_FORMAT_R32_UINT;
    case rhi::BasicFormat::R32_SINT:            return VK_FORMAT_R32_SINT;
    case rhi::BasicFormat::R8G8_UINT:           return VK_FORMAT_R8G8_UINT;
    case rhi::BasicFormat::R8G8_SINT:           return VK_FORMAT_R8G8_SINT;
    case rhi::BasicFormat::R8G8_UNORM:          return VK_FORMAT_R8G8_UNORM;
    case rhi::BasicFormat::R8G8_SNORM:          return VK_FORMAT_R8G8_SNORM;
    case rhi::BasicFormat::R16_FLOAT:           return VK_FORMAT_R16_SFLOAT;
    case rhi::BasicFormat::R16_UINT:            return VK_FORMAT_R16_UINT;
    case rhi::BasicFormat::R16_SINT:            return VK_FORMAT_R16_SINT;
    case rhi::BasicFormat::R16_UNORM:           return VK_FORMAT_R16_UNORM;
    case rhi::BasicFormat::R16_SNORM:           return VK_FORMAT_R16_SNORM;
    case rhi::BasicFormat::R8_UINT:             return VK_FORMAT_R8_UINT;
    case rhi::BasicFormat::R8_SINT:             return VK_FORMAT_R8_SINT;
    case rhi::BasicFormat::R8_UNORM:            return VK_FORMAT_R8_UNORM;
    case rhi::BasicFormat::R8_SNORM:            return VK_FORMAT_R8_SNORM;
    case rhi::BasicFormat::D32_FLOAT:           return VK_FORMAT_D32_SFLOAT;
    case rhi::BasicFormat::D24_UNORM_S8_UINT:   return VK_FORMAT_D24_UNORM_S8_UINT;
    case rhi::BasicFormat::D16_UNORM:           return VK_FORMAT_D16_UNORM;
    case rhi::BasicFormat::B5G6R5_UNORM:        return VK_FORMAT_B5G6R5_UNORM_PACK16;
    case rhi::BasicFormat::B5G5R5A1_UNORM:      return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    case rhi::BasicFormat::B8G8R8A8_UNORM:      return VK_FORMAT_B8G8R8A8_UNORM;
    case rhi::BasicFormat::B8G8R8X8_UNORM:      return VK_FORMAT_B8G8R8_UNORM;
    case rhi::BasicFormat::B8G8R8A8_UNORM_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
    case rhi::BasicFormat::B8G8R8X8_UNORM_SRGB: return VK_FORMAT_B8G8R8_SRGB;
    }
    CONVERT_UNSUPPORTED("ConvertBasicFormat not supported, "
        "input format is %d", gp::EnumCast(format));
    return VK_FORMAT_UNDEFINED;
}

VkFormat ConvertIndexFormat(rhi::IndexFormat format)
{
    switch (format) {
    case rhi::IndexFormat::UINT16: return VK_FORMAT_R16_UINT;
    case rhi::IndexFormat::UINT32: return VK_FORMAT_R32_UINT;
    }
    CONVERT_UNSUPPORTED("ConvertIndexFormat not supported, "
        "input format is %d", gp::EnumCast(format));
    return VK_FORMAT_UNDEFINED;
}

VkFormat ConvertVertexFormat(rhi::VertexFormat format)
{
    switch (format) {
    case rhi::VertexFormat::UINT8:        return VK_FORMAT_R8_UINT;
    case rhi::VertexFormat::UINT8_VEC2:   return VK_FORMAT_R8G8_UINT;
    case rhi::VertexFormat::UINT8_VEC3:   return VK_FORMAT_R8G8B8_UINT;
    case rhi::VertexFormat::UINT8_VEC4:   return VK_FORMAT_R8G8B8A8_UINT;
    case rhi::VertexFormat::SINT8:        return VK_FORMAT_R8_SINT;
    case rhi::VertexFormat::SINT8_VEC2:   return VK_FORMAT_R8G8_SINT;
    case rhi::VertexFormat::SINT8_VEC3:   return VK_FORMAT_R8G8B8_SINT;
    case rhi::VertexFormat::SINT8_VEC4:   return VK_FORMAT_R8G8B8A8_SINT;
    case rhi::VertexFormat::UINT16:       return VK_FORMAT_R16_UINT;
    case rhi::VertexFormat::UINT16_VEC2:  return VK_FORMAT_R16G16_UINT;
    case rhi::VertexFormat::UINT16_VEC3:  return VK_FORMAT_R16G16B16_UINT;
    case rhi::VertexFormat::UINT16_VEC4:  return VK_FORMAT_R16G16B16A16_UINT;
    case rhi::VertexFormat::SINT16:       return VK_FORMAT_R16_SINT;
    case rhi::VertexFormat::SINT16_VEC2:  return VK_FORMAT_R16G16_SINT;
    case rhi::VertexFormat::SINT16_VEC3:  return VK_FORMAT_R16G16B16_SINT;
    case rhi::VertexFormat::SINT16_VEC4:  return VK_FORMAT_R16G16B16A16_SINT;
    case rhi::VertexFormat::FLOAT16:      return VK_FORMAT_R16_SFLOAT;
    case rhi::VertexFormat::FLOAT16_VEC2: return VK_FORMAT_R16G16_SFLOAT;
    case rhi::VertexFormat::FLOAT16_VEC3: return VK_FORMAT_R16G16B16_SFLOAT;
    case rhi::VertexFormat::FLOAT16_VEC4: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case rhi::VertexFormat::UINT32:       return VK_FORMAT_R32_UINT;
    case rhi::VertexFormat::UINT32_VEC2:  return VK_FORMAT_R32G32_UINT;
    case rhi::VertexFormat::UINT32_VEC3:  return VK_FORMAT_R32G32B32_UINT;
    case rhi::VertexFormat::UINT32_VEC4:  return VK_FORMAT_R32G32B32A32_UINT;
    case rhi::VertexFormat::SINT32:       return VK_FORMAT_R32_SINT;
    case rhi::VertexFormat::SINT32_VEC2:  return VK_FORMAT_R32G32_SINT;
    case rhi::VertexFormat::SINT32_VEC3:  return VK_FORMAT_R32G32B32_SINT;
    case rhi::VertexFormat::SINT32_VEC4:  return VK_FORMAT_R32G32B32A32_SINT;
    case rhi::VertexFormat::FLOAT32:      return VK_FORMAT_R32_SFLOAT;
    case rhi::VertexFormat::FLOAT32_VEC2: return VK_FORMAT_R32G32_SFLOAT;
    case rhi::VertexFormat::FLOAT32_VEC3: return VK_FORMAT_R32G32B32_SFLOAT;
    case rhi::VertexFormat::FLOAT32_VEC4: return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    CONVERT_UNSUPPORTED("ConvertVertexFormat not supported, "
        "input format is %d", gp::EnumCast(format));
    return VK_FORMAT_UNDEFINED;
}

VkVertexInputRate ConvertInputClassification(rhi::VertexInputRate rate)
{
    switch (rate) {
    case rhi::VertexInputRate::PER_VERTEX:   return VK_VERTEX_INPUT_RATE_VERTEX;
    case rhi::VertexInputRate::PER_INSTANCE: return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
    CONVERT_UNSUPPORTED("ConvertInputClassification not supported, "
        "input rate is %d", gp::EnumCast(rate));
    return VK_VERTEX_INPUT_RATE_VERTEX;
}

VkPrimitiveTopology ConvertPrimitiveTopology(rhi::PrimitiveTopology topology)
{
    switch (topology) {
    case rhi::PrimitiveTopology::POINT_LIST:     return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case rhi::PrimitiveTopology::LINE_LIST:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case rhi::PrimitiveTopology::LINE_STRIP:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case rhi::PrimitiveTopology::TRIANGLE_LIST:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case rhi::PrimitiveTopology::TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    }
    CONVERT_UNSUPPORTED("ConvertPrimitiveTopology not supported, "
        "input topology is %d", gp::EnumCast(topology));
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

VkMemoryPropertyFlags ConvertHeap(rhi::TransferDirection type)
{
    switch (type) {
    case rhi::TransferDirection::CPU_TO_GPU:
        return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    case rhi::TransferDirection::GPU_TO_CPU:
        return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    case rhi::TransferDirection::GPU_ONLY:
        return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    CONVERT_UNSUPPORTED("ConvertHeap not supported, "
        "input type is %d", gp::EnumCast(type));
    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
}

VkImageLayout ConvertImageLayout(rhi::ResourceState state)
{
    switch (state) {
    case rhi::ResourceState::UNDEFINED:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    case rhi::ResourceState::PRESENT:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case rhi::ResourceState::COLOR_OUTPUT:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case rhi::ResourceState::GENERAL_READ:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case rhi::ResourceState::GENERAL_READ_WRITE:
        return VK_IMAGE_LAYOUT_GENERAL;
    case rhi::ResourceState::DEPTH_STENCIL_READ:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    case rhi::ResourceState::DEPTH_STENCIL_WRITE:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case rhi::ResourceState::COPY_SOURCE:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case rhi::ResourceState::COPY_DESTINATION:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
    CONVERT_UNSUPPORTED("ConvertImageLayout not supported, "
        "input state is %d", gp::EnumCast(state));
    return VK_IMAGE_LAYOUT_GENERAL;
}

VkAccessFlags ConvertAccessFlags(rhi::ResourceState state)
{
    switch (state) {
    case rhi::ResourceState::UNDEFINED:
    case rhi::ResourceState::PRESENT:
        return 0; // VK_ACCESS_NONE_KHR
    case rhi::ResourceState::COLOR_OUTPUT:
        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case rhi::ResourceState::GENERAL_READ:
        return VK_ACCESS_SHADER_READ_BIT;
    case rhi::ResourceState::GENERAL_READ_WRITE:
        return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    case rhi::ResourceState::DEPTH_STENCIL_READ:
        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    case rhi::ResourceState::DEPTH_STENCIL_WRITE:
        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    case rhi::ResourceState::COPY_SOURCE:
        return VK_ACCESS_TRANSFER_READ_BIT;
    case rhi::ResourceState::COPY_DESTINATION:
        return VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    CONVERT_UNSUPPORTED("ConvertAccessFlags not supported, "
        "input state is %d", gp::EnumCast(state));
    return 0;
}

VkPipelineStageFlags ConvertPipelineStageFlags(rhi::ResourceState state)
{
    switch (state) {
    case rhi::ResourceState::UNDEFINED:
        return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    case rhi::ResourceState::PRESENT:
        return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    case rhi::ResourceState::COLOR_OUTPUT:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case rhi::ResourceState::GENERAL_READ:
    case rhi::ResourceState::GENERAL_READ_WRITE:
        return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
             | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
             | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    case rhi::ResourceState::DEPTH_STENCIL_READ:
        return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    case rhi::ResourceState::DEPTH_STENCIL_WRITE:
        return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
             | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    case rhi::ResourceState::COPY_SOURCE:
    case rhi::ResourceState::COPY_DESTINATION:
        return VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    CONVERT_UNSUPPORTED("ConvertPipelineStageFlags not supported, "
        "input state is %d", gp::EnumCast(state));
    return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
}

VkImageUsageFlags ConvertImageUsageFlags(rhi::ImageType type)
{
    VkImageUsageFlags flags = VK_IMAGE_USAGE_SAMPLED_BIT
                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                            | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    switch (type) {
    case rhi::ImageType::ShaderResource:
        return flags;
    case rhi::ImageType::Color:
        return flags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    case rhi::ImageType::Depth:
    case rhi::ImageType::Stencil:
    case rhi::ImageType::DepthStencil:
        return flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    CONVERT_UNSUPPORTED("ConvertImageUsageFlags not supported, "
        "input type is %d", gp::EnumCast(type));
    return flags;
}

VkImageType ConvertImageType(rhi::ImageDimension dimension)
{
    switch (dimension) {
    case rhi::ImageDimension::Dimension1D: return VK_IMAGE_TYPE_1D;
    case rhi::ImageDimension::Dimension2D: return VK_IMAGE_TYPE_2D;
    case rhi::ImageDimension::Dimension3D: return VK_IMAGE_TYPE_3D;
    }
    CONVERT_UNSUPPORTED("ConvertImageType not supported, "
        "input dimension is %d", gp::EnumCast(dimension));
    return VK_IMAGE_TYPE_2D;
}

VkImageViewType ConvertImageViewType(rhi::ImageDimension dimension)
{
    switch (dimension) {
    case rhi::ImageDimension::Dimension1D: return VK_IMAGE_VIEW_TYPE_1D;
    case rhi::ImageDimension::Dimension2D: return VK_IMAGE_VIEW_TYPE_2D;
    case rhi::ImageDimension::Dimension3D: return VK_IMAGE_VIEW_TYPE_3D;
    }
    CONVERT_UNSUPPORTED("ConvertImageViewType not supported, "
        "input dimension is %d", gp::EnumCast(dimension));
    return VK_IMAGE_VIEW_TYPE_2D;
}

VkDescriptorType ConvertDescriptorType(rhi::DescriptorType type)
{
    switch (type) {
    case rhi::DescriptorType::UniformBuffer:    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case rhi::DescriptorType::ReadOnlyBuffer:   return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case rhi::DescriptorType::ReadWriteBuffer:  return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case rhi::DescriptorType::ReadOnlyTexture:  return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case rhi::DescriptorType::ReadWriteTexture: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case rhi::DescriptorType::ImageSampler:     return VK_DESCRIPTOR_TYPE_SAMPLER;
    case rhi::DescriptorType::ColorOutput:      return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; // is it need?
    case rhi::DescriptorType::DepthStencil:     return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; // is it need?
    }
    CONVERT_UNSUPPORTED("ConvertDescriptorType not supported, "
        "input type is %d", gp::EnumCast(type));
    return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
}

VkShaderStageFlags ConvertShaderStage(rhi::ShaderStage stage)
{
    switch (stage) {
    case rhi::ShaderStage::Vertex:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case rhi::ShaderStage::Hull:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case rhi::ShaderStage::Domain:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case rhi::ShaderStage::Geometry:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    case rhi::ShaderStage::Pixel:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case rhi::ShaderStage::Compute:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    case rhi::ShaderStage::Graphics:
        return VK_SHADER_STAGE_ALL_GRAPHICS;
    }
    CONVERT_UNSUPPORTED("ConvertShaderStage not supported, "
        "input stage is %d", gp::EnumCast(stage));
    return VK_SHADER_STAGE_ALL;
}

VkClearValue ConvertClearValue(rhi::BasicFormat format, const rhi::ClearValue& value)
{
    VkClearValue clearValue = {};
    if (IsBasicFormatHasDepth(format) || IsBasicFormatHasStencil(format)) {
        clearValue.depthStencil.depth = value.image.depth;
        clearValue.depthStencil.stencil = value.image.stencil;
    } else {
        clearValue.color.float32[0] = value.image.color[0];
        clearValue.color.float32[1] = value.image.color[1];
        clearValue.color.float32[2] = value.image.color[2];
        clearValue.color.float32[3] = value.image.color[3];
    }
    return clearValue;
}

VkPolygonMode ConvertFillMode(rhi::FillMode mode)
{
    switch (mode) {
    case rhi::FillMode::Wireframe: return VK_POLYGON_MODE_LINE;
    case rhi::FillMode::Solid:     return VK_POLYGON_MODE_FILL;
    }
    CONVERT_UNSUPPORTED("ConvertFillMode not supported, "
        "input mode is %d", gp::EnumCast(mode));
    return VK_POLYGON_MODE_FILL;
}

VkCullModeFlags ConvertCullMode(rhi::CullMode mode)
{
    switch (mode) {
    case rhi::CullMode::None:  return VK_CULL_MODE_NONE;
    case rhi::CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
    case rhi::CullMode::Back:  return VK_CULL_MODE_BACK_BIT;
    }
    CONVERT_UNSUPPORTED("ConvertCullMode not supported, "
        "input mode is %d", gp::EnumCast(mode));
    return VK_CULL_MODE_NONE;
}

VkSamplerAddressMode ConvertAddressMode(rhi::AddressMode mode)
{
    switch (mode) {
    case rhi::AddressMode::Wrap:   return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case rhi::AddressMode::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case rhi::AddressMode::Clamp:  return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case rhi::AddressMode::Border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    }
    CONVERT_UNSUPPORTED("ConvertAddressMode not supported, "
        "input mode is %d", gp::EnumCast(mode));
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

VkFilter ConvertFilter(rhi::SamplerState::Filter filter)
{
    switch (filter) {
    case rhi::SamplerState::Filter::Point:       return VK_FILTER_NEAREST;
    case rhi::SamplerState::Filter::Linear:      return VK_FILTER_LINEAR;
    }
    CONVERT_UNSUPPORTED("ConvertFilter not supported, "
        "input filter is %d", gp::EnumCast(filter));
    return VK_FILTER_LINEAR;
}

VkSamplerMipmapMode ConvertMipmapMode(rhi::SamplerState::Filter filter)
{
    switch (filter) {
    case rhi::SamplerState::Filter::Point:  return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case rhi::SamplerState::Filter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
    CONVERT_UNSUPPORTED("ConvertMipmapMode not supported, "
        "input filter is %d", gp::EnumCast(filter));
    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

VkAttachmentLoadOp ConvertAttachmentLoadOp(rhi::PassAction action)
{
    switch (action) {
    case rhi::PassAction::Load:
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    case rhi::PassAction::Clear:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case rhi::PassAction::Discard:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
    CONVERT_UNSUPPORTED("ConvertAttachmentLoadOp not supported, "
        "input action is %d", gp::EnumCast(action));
    return VK_ATTACHMENT_LOAD_OP_LOAD;
}

VkAttachmentStoreOp ConvertAttachmentStoreOp(rhi::PassAction action)
{
    switch (action) {
    case rhi::PassAction::Store:
        return VK_ATTACHMENT_STORE_OP_STORE;
    case rhi::PassAction::Discard:
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
    CONVERT_UNSUPPORTED("ConvertAttachmentStoreOp not supported, "
        "input action is %d", gp::EnumCast(action));
    return VK_ATTACHMENT_STORE_OP_STORE;
}

}
