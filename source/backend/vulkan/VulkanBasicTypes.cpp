#include "VulkanBasicTypes.h"

namespace au::backend {

VkFormat ConvertBasicFormat(rhi::BasicFormat format)
{
    switch (format) {
        case rhi::BasicFormat::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
        case rhi::BasicFormat::B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
        case rhi::BasicFormat::R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case rhi::BasicFormat::R32G32B32_FLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        case rhi::BasicFormat::R32G32_FLOAT: return VK_FORMAT_R32G32_SFLOAT;
        case rhi::BasicFormat::R32_FLOAT: return VK_FORMAT_R32_SFLOAT;
        case rhi::BasicFormat::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
        case rhi::BasicFormat::D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkFormat ConvertIndexFormat(rhi::IndexFormat format)
{
    switch (format) {
        case rhi::IndexFormat::UINT16: return VK_FORMAT_R16_UINT;
        case rhi::IndexFormat::UINT32: return VK_FORMAT_R32_UINT;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkFormat ConvertVertexFormat(rhi::VertexFormat format)
{
    switch (format) {
        case rhi::VertexFormat::UINT8: return VK_FORMAT_R8_UINT;
        case rhi::VertexFormat::UINT8_VEC2: return VK_FORMAT_R8G8_UINT;
        case rhi::VertexFormat::UINT8_VEC3: return VK_FORMAT_R8G8B8_UINT;
        case rhi::VertexFormat::UINT8_VEC4: return VK_FORMAT_R8G8B8A8_UINT;
        case rhi::VertexFormat::SINT8: return VK_FORMAT_R8_SINT;
        case rhi::VertexFormat::SINT8_VEC2: return VK_FORMAT_R8G8_SINT;
        case rhi::VertexFormat::SINT8_VEC3: return VK_FORMAT_R8G8B8_SINT;
        case rhi::VertexFormat::SINT8_VEC4: return VK_FORMAT_R8G8B8A8_SINT;
        case rhi::VertexFormat::UINT16: return VK_FORMAT_R16_UINT;
        case rhi::VertexFormat::UINT16_VEC2: return VK_FORMAT_R16G16_UINT;
        case rhi::VertexFormat::UINT16_VEC3: return VK_FORMAT_R16G16B16_UINT;
        case rhi::VertexFormat::UINT16_VEC4: return VK_FORMAT_R16G16B16A16_UINT;
        case rhi::VertexFormat::SINT16: return VK_FORMAT_R16_SINT;
        case rhi::VertexFormat::SINT16_VEC2: return VK_FORMAT_R16G16_SINT;
        case rhi::VertexFormat::SINT16_VEC3: return VK_FORMAT_R16G16B16_SINT;
        case rhi::VertexFormat::SINT16_VEC4: return VK_FORMAT_R16G16B16A16_SINT;
        case rhi::VertexFormat::FLOAT16: return VK_FORMAT_R16_SFLOAT;
        case rhi::VertexFormat::FLOAT16_VEC2: return VK_FORMAT_R16G16_SFLOAT;
        case rhi::VertexFormat::FLOAT16_VEC3: return VK_FORMAT_R16G16B16_SFLOAT;
        case rhi::VertexFormat::FLOAT16_VEC4: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case rhi::VertexFormat::UINT32: return VK_FORMAT_R32_UINT;
        case rhi::VertexFormat::UINT32_VEC2: return VK_FORMAT_R32G32_UINT;
        case rhi::VertexFormat::UINT32_VEC3: return VK_FORMAT_R32G32B32_UINT;
        case rhi::VertexFormat::UINT32_VEC4: return VK_FORMAT_R32G32B32A32_UINT;
        case rhi::VertexFormat::SINT32: return VK_FORMAT_R32_SINT;
        case rhi::VertexFormat::SINT32_VEC2: return VK_FORMAT_R32G32_SINT;
        case rhi::VertexFormat::SINT32_VEC3: return VK_FORMAT_R32G32B32_SINT;
        case rhi::VertexFormat::SINT32_VEC4: return VK_FORMAT_R32G32B32A32_SINT;
        case rhi::VertexFormat::FLOAT32: return VK_FORMAT_R32_SFLOAT;
        case rhi::VertexFormat::FLOAT32_VEC2: return VK_FORMAT_R32G32_SFLOAT;
        case rhi::VertexFormat::FLOAT32_VEC3: return VK_FORMAT_R32G32B32_SFLOAT;
        case rhi::VertexFormat::FLOAT32_VEC4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkVertexInputRate ConvertInputClassification(rhi::VertexInputRate rate)
{
    switch (rate) {
        case rhi::VertexInputRate::PER_VERTEX: return VK_VERTEX_INPUT_RATE_VERTEX;
        case rhi::VertexInputRate::PER_INSTANCE: return VK_VERTEX_INPUT_RATE_INSTANCE;
        default: return VK_VERTEX_INPUT_RATE_VERTEX;
    }
}

VkPrimitiveTopology ConvertPrimitiveTopology(rhi::PrimitiveTopology topology)
{
    switch (topology) {
        case rhi::PrimitiveTopology::POINT_LIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case rhi::PrimitiveTopology::LINE_LIST: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case rhi::PrimitiveTopology::LINE_STRIP: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case rhi::PrimitiveTopology::TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case rhi::PrimitiveTopology::TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkMemoryPropertyFlags ConvertHeap(rhi::TransferDirection type)
{
    switch (type) {
        case rhi::TransferDirection::CPU_TO_GPU:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        case rhi::TransferDirection::GPU_TO_CPU:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        case rhi::TransferDirection::GPU_ONLY:
        default:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
}

VkImageLayout ConvertImageLayout(rhi::ResourceState state)
{
    switch (state) {
        case rhi::ResourceState::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
        case rhi::ResourceState::COLOR_OUTPUT: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case rhi::ResourceState::DEPTH_STENCIL_WRITE: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case rhi::ResourceState::DEPTH_STENCIL_READ: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case rhi::ResourceState::GENERAL_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case rhi::ResourceState::GENERAL_READ_WRITE: return VK_IMAGE_LAYOUT_GENERAL;
        case rhi::ResourceState::COPY_SOURCE: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case rhi::ResourceState::COPY_DESTINATION: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case rhi::ResourceState::PRESENT: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default: return VK_IMAGE_LAYOUT_GENERAL;
    }
}

VkAccessFlags ConvertAccessFlags(rhi::ResourceState state)
{
    switch (state) {
        case rhi::ResourceState::COLOR_OUTPUT: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case rhi::ResourceState::DEPTH_STENCIL_WRITE: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case rhi::ResourceState::DEPTH_STENCIL_READ: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        case rhi::ResourceState::GENERAL_READ: return VK_ACCESS_SHADER_READ_BIT;
        case rhi::ResourceState::GENERAL_READ_WRITE: return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        case rhi::ResourceState::COPY_SOURCE: return VK_ACCESS_TRANSFER_READ_BIT;
        case rhi::ResourceState::COPY_DESTINATION: return VK_ACCESS_TRANSFER_WRITE_BIT;
        case rhi::ResourceState::PRESENT: return 0;
        default: return 0;
    }
}

VkPipelineStageFlags ConvertPipelineStageFlags(rhi::ResourceState state)
{
    switch (state) {
        case rhi::ResourceState::COLOR_OUTPUT: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case rhi::ResourceState::DEPTH_STENCIL_WRITE: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        case rhi::ResourceState::DEPTH_STENCIL_READ: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case rhi::ResourceState::GENERAL_READ: return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case rhi::ResourceState::GENERAL_READ_WRITE: return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        case rhi::ResourceState::COPY_SOURCE: return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case rhi::ResourceState::COPY_DESTINATION: return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case rhi::ResourceState::PRESENT: return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        default: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
}

VkImageUsageFlags ConvertImageUsageFlags(rhi::ImageType type)
{
    VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    
    if (type & rhi::ImageType::Color) {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if (type & rhi::ImageType::Depth) {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (type & rhi::ImageType::Stencil) {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (type == rhi::ImageType::ShaderResource) {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    
    return flags;
}

VkImageType ConvertImageType(rhi::ImageDimension dimension)
{
    switch (dimension) {
        case rhi::ImageDimension::Dimension1D: return VK_IMAGE_TYPE_1D;
        case rhi::ImageDimension::Dimension2D: return VK_IMAGE_TYPE_2D;
        case rhi::ImageDimension::Dimension3D: return VK_IMAGE_TYPE_3D;
        default: return VK_IMAGE_TYPE_2D;
    }
}

VkImageViewType ConvertImageViewType(rhi::ImageDimension dimension)
{
    switch (dimension) {
        case rhi::ImageDimension::Dimension1D: return VK_IMAGE_VIEW_TYPE_1D;
        case rhi::ImageDimension::Dimension2D: return VK_IMAGE_VIEW_TYPE_2D;
        case rhi::ImageDimension::Dimension3D: return VK_IMAGE_VIEW_TYPE_3D;
        default: return VK_IMAGE_VIEW_TYPE_2D;
    }
}

VkDescriptorType ConvertDescriptorType(rhi::DescriptorType type)
{
    // For combined flags, prioritize based on common usage
    if (type & rhi::DescriptorType::UniformBuffer) {
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
    if (type & rhi::DescriptorType::ReadOnlyBuffer) {
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }
    if (type & rhi::DescriptorType::ReadWriteBuffer) {
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }
    if (type & rhi::DescriptorType::ReadOnlyTexture) {
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    }
    if (type & rhi::DescriptorType::ReadWriteTexture) {
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    }
    if (type & rhi::DescriptorType::ImageSampler) {
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    }
    
    // Default case
    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

VkShaderStageFlags ConvertShaderStage(rhi::ShaderStage stage)
{
    VkShaderStageFlags flags = 0;
    
    if (stage & rhi::ShaderStage::Vertex) flags |= VK_SHADER_STAGE_VERTEX_BIT;
    if (stage & rhi::ShaderStage::Hull) flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if (stage & rhi::ShaderStage::Domain) flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    if (stage & rhi::ShaderStage::Geometry) flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    if (stage & rhi::ShaderStage::Pixel) flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if (stage & rhi::ShaderStage::Compute) flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    
    return flags;
}

VkClearValue ConvertClearValue(rhi::BasicFormat format, const rhi::ClearValue& value)
{
    VkClearValue clearValue = {};
    
    // Check if it's a depth-stencil format
    if (format == rhi::BasicFormat::D24_UNORM_S8_UINT || format == rhi::BasicFormat::D32_FLOAT || format == rhi::BasicFormat::D16_UNORM) {
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
        case rhi::FillMode::Solid: return VK_POLYGON_MODE_FILL;
        default: return VK_POLYGON_MODE_FILL;
    }
}

VkCullModeFlags ConvertCullMode(rhi::CullMode mode)
{
    switch (mode) {
        case rhi::CullMode::None: return VK_CULL_MODE_NONE;
        case rhi::CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
        case rhi::CullMode::Back: return VK_CULL_MODE_BACK_BIT;
        default: return VK_CULL_MODE_NONE;
    }
}

VkSamplerAddressMode ConvertAddressMode(rhi::AddressMode mode)
{
    switch (mode) {
        case rhi::AddressMode::Wrap: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case rhi::AddressMode::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case rhi::AddressMode::Clamp: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case rhi::AddressMode::Border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

VkFilter ConvertFilter(rhi::SamplerState::Filter filter)
{
    switch (filter) {
        case rhi::SamplerState::Filter::Point: return VK_FILTER_NEAREST;
        case rhi::SamplerState::Filter::Linear: return VK_FILTER_LINEAR;
        case rhi::SamplerState::Filter::Anisotropic: return VK_FILTER_LINEAR; // Use linear for anisotropic base
        default: return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode ConvertMipmapMode(rhi::SamplerState::Filter filter)
{
    switch (filter) {
        case rhi::SamplerState::Filter::Point: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case rhi::SamplerState::Filter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        case rhi::SamplerState::Filter::Anisotropic: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

VkAttachmentLoadOp ConvertLoadOp(rhi::PassAction action)
{
    if (action & rhi::PassAction::Clear) {
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    if (action & rhi::PassAction::Load) {
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    }
    if (action & rhi::PassAction::Discard) {
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp ConvertStoreOp(rhi::PassAction action)
{
    if (action & rhi::PassAction::Store) {
        return VK_ATTACHMENT_STORE_OP_STORE;
    }
    if (action & rhi::PassAction::Discard) {
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
    return VK_ATTACHMENT_STORE_OP_STORE;
}

}