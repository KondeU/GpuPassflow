#include "VulkanVertexBuffer.h"
#include "VulkanVertexAttribute.h"
#include "VulkanIndexBuffer.h"
#include "VulkanIndexAttribute.h"
#include "VulkanConstantBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanStorageBuffer.h"
#include "VulkanImageBuffer.h"
#include "VulkanImageSampler.h"
#include "VulkanDescriptorHeap.h"
#include "VulkanDescriptorGroup.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipelineState.h"

namespace au::backend {

// VulkanVertexBuffer is now implemented in its own file

// VulkanVertexAttribute
VulkanVertexAttribute::VulkanVertexAttribute() {}
VulkanVertexAttribute::~VulkanVertexAttribute() {}

// VulkanIndexBuffer is now implemented in its own file

// VulkanIndexAttribute
VulkanIndexAttribute::VulkanIndexAttribute() {}
VulkanIndexAttribute::~VulkanIndexAttribute() {}

// VulkanConstantBuffer
VulkanConstantBuffer::VulkanConstantBuffer() {}
VulkanConstantBuffer::~VulkanConstantBuffer() {}

// VulkanUniformBuffer
VulkanUniformBuffer::VulkanUniformBuffer() {}
VulkanUniformBuffer::~VulkanUniformBuffer() {}
bool VulkanUniformBuffer::Setup(Description description) { return true; }
VkBuffer VulkanUniformBuffer::GetBuffer() const { return buffer; }

// VulkanStorageBuffer
VulkanStorageBuffer::VulkanStorageBuffer() {}
VulkanStorageBuffer::~VulkanStorageBuffer() {}
bool VulkanStorageBuffer::Setup(Description description) { return true; }
VkBuffer VulkanStorageBuffer::GetBuffer() const { return buffer; }

// VulkanImageBuffer
VulkanImageBuffer::VulkanImageBuffer() {}
VulkanImageBuffer::~VulkanImageBuffer() {}
bool VulkanImageBuffer::Setup(Description description) { return true; }
VkImage VulkanImageBuffer::GetImage() const { return image; }
VkImageView VulkanImageBuffer::GetImageView() const { return imageView; }

// VulkanImageSampler
VulkanImageSampler::VulkanImageSampler() {}
VulkanImageSampler::~VulkanImageSampler() {}
bool VulkanImageSampler::Setup(Description description) { return true; }
VkSampler VulkanImageSampler::GetSampler() const { return sampler; }

// VulkanDescriptorHeap
VulkanDescriptorHeap::VulkanDescriptorHeap() {}
VulkanDescriptorHeap::~VulkanDescriptorHeap() {}
bool VulkanDescriptorHeap::Setup(Description description) { return true; }
VkDescriptorPool VulkanDescriptorHeap::GetDescriptorPool() const { return descriptorPool; }

// VulkanDescriptorGroup
VulkanDescriptorGroup::VulkanDescriptorGroup() {}
VulkanDescriptorGroup::~VulkanDescriptorGroup() {}
bool VulkanDescriptorGroup::Setup(Description description) { return true; }
VkDescriptorSet VulkanDescriptorGroup::GetDescriptorSet() const { return descriptorSet; }

// VulkanPipelineLayout
VulkanPipelineLayout::VulkanPipelineLayout() {}
VulkanPipelineLayout::~VulkanPipelineLayout() {}
bool VulkanPipelineLayout::Setup(Description description) { return true; }
VkPipelineLayout VulkanPipelineLayout::GetPipelineLayout() const { return pipelineLayout; }

// VulkanPipelineState
VulkanPipelineState::VulkanPipelineState() {}
VulkanPipelineState::~VulkanPipelineState() {}
bool VulkanPipelineState::Setup(Description description) { return true; }
VkPipeline VulkanPipelineState::GetPipeline() const { return pipeline; }

} 