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
        case rhi::IndexFormat::UInt16: return VK_FORMAT_R16_UINT;
        case rhi::IndexFormat::UInt32: return VK_FORMAT_R32_UINT;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkFormat ConvertVertexFormat(rhi::VertexFormat format)
{
    switch (format) {
        case rhi::VertexFormat::Float: return VK_FORMAT_R32_SFLOAT;
        case rhi::VertexFormat::Float2: return VK_FORMAT_R32G32_SFLOAT;
        case rhi::VertexFormat::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
        case rhi::VertexFormat::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkVertexInputRate ConvertInputClassification(rhi::VertexInputRate rate)
{
    switch (rate) {
        case rhi::VertexInputRate::Vertex: return VK_VERTEX_INPUT_RATE_VERTEX;
        case rhi::VertexInputRate::Instance: return VK_VERTEX_INPUT_RATE_INSTANCE;
        default: return VK_VERTEX_INPUT_RATE_VERTEX;
    }
}

VkPrimitiveTopology ConvertPrimitiveTopology(rhi::PrimitiveTopology topology)
{
    switch (topology) {
        case rhi::PrimitiveTopology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case rhi::PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case rhi::PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case rhi::PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case rhi::PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkMemoryPropertyFlags ConvertHeap(rhi::TransferDirection type)
{
    switch (type) {
        case rhi::TransferDirection::Upload:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        case rhi::TransferDirection::Readback:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        case rhi::TransferDirection::Default:
        default:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
}

VkImageLayout ConvertImageLayout(rhi::ResourceState state)
{
    switch (state) {
        case rhi::ResourceState::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
        case rhi::ResourceState::RenderTarget: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case rhi::ResourceState::DepthWrite: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case rhi::ResourceState::ShaderResource: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case rhi::ResourceState::Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default: return VK_IMAGE_LAYOUT_GENERAL;
    }
}

VkAccessFlags ConvertAccessFlags(rhi::ResourceState state)
{
    switch (state) {
        case rhi::ResourceState::RenderTarget: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case rhi::ResourceState::DepthWrite: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case rhi::ResourceState::ShaderResource: return VK_ACCESS_SHADER_READ_BIT;
        case rhi::ResourceState::Present: return 0;
        default: return 0;
    }
}

VkPipelineStageFlags ConvertPipelineStageFlags(rhi::ResourceState state)
{
    switch (state) {
        case rhi::ResourceState::RenderTarget: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case rhi::ResourceState::DepthWrite: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case rhi::ResourceState::ShaderResource: return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case rhi::ResourceState::Present: return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        default: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
}

VkImageUsageFlags ConvertImageUsageFlags(rhi::ImageType type)
{
    VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    
    switch (type) {
        case rhi::ImageType::RenderTarget:
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            break;
        case rhi::ImageType::DepthStencil:
            flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            break;
        case rhi::ImageType::Texture:
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
            break;
        default:
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
            break;
    }
    
    return flags;
}

VkImageType ConvertImageType(rhi::ImageDimension dimension)
{
    switch (dimension) {
        case rhi::ImageDimension::Image1D: return VK_IMAGE_TYPE_1D;
        case rhi::ImageDimension::Image2D: return VK_IMAGE_TYPE_2D;
        case rhi::ImageDimension::Image3D: return VK_IMAGE_TYPE_3D;
        default: return VK_IMAGE_TYPE_2D;
    }
}

VkImageViewType ConvertImageViewType(rhi::ImageDimension dimension)
{
    switch (dimension) {
        case rhi::ImageDimension::Image1D: return VK_IMAGE_VIEW_TYPE_1D;
        case rhi::ImageDimension::Image2D: return VK_IMAGE_VIEW_TYPE_2D;
        case rhi::ImageDimension::Image3D: return VK_IMAGE_VIEW_TYPE_3D;
        default: return VK_IMAGE_VIEW_TYPE_2D;
    }
}

VkDescriptorType ConvertDescriptorType(rhi::DescriptorType type)
{
    switch (type) {
        case rhi::DescriptorType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case rhi::DescriptorType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case rhi::DescriptorType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case rhi::DescriptorType::SampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case rhi::DescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

VkShaderStageFlags ConvertShaderStage(rhi::ShaderStage stage)
{
    VkShaderStageFlags flags = 0;
    
    if (stage & rhi::ShaderStage::Vertex) flags |= VK_SHADER_STAGE_VERTEX_BIT;
    if (stage & rhi::ShaderStage::Fragment) flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if (stage & rhi::ShaderStage::Geometry) flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    if (stage & rhi::ShaderStage::Compute) flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    
    return flags;
}

VkClearValue ConvertClearValue(rhi::BasicFormat format, rhi::ClearValue value)
{
    VkClearValue clearValue = {};
    
    // Check if it's a depth-stencil format
    if (format == rhi::BasicFormat::D24_UNORM_S8_UINT || format == rhi::BasicFormat::D32_FLOAT) {
        clearValue.depthStencil.depth = value.depth;
        clearValue.depthStencil.stencil = value.stencil;
    } else {
        clearValue.color.float32[0] = value.color[0];
        clearValue.color.float32[1] = value.color[1];
        clearValue.color.float32[2] = value.color[2];
        clearValue.color.float32[3] = value.color[3];
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

VkFilter ConvertFilter(rhi::Filter filter)
{
    switch (filter) {
        case rhi::Filter::Point: return VK_FILTER_NEAREST;
        case rhi::Filter::Linear: return VK_FILTER_LINEAR;
        default: return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode ConvertMipmapMode(rhi::Filter filter)
{
    switch (filter) {
        case rhi::Filter::Point: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case rhi::Filter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

VkAttachmentLoadOp ConvertLoadOp(rhi::PassAction action)
{
    switch (action) {
        case rhi::PassAction::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case rhi::PassAction::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
        case rhi::PassAction::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

VkAttachmentStoreOp ConvertStoreOp(rhi::PassAction action)
{
    switch (action) {
        case rhi::PassAction::Store: return VK_ATTACHMENT_STORE_OP_STORE;
        case rhi::PassAction::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default: return VK_ATTACHMENT_STORE_OP_STORE;
    }
}

} 