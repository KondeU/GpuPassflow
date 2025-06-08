#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanDescriptorGroup : public rhi::DescriptorGroup
    , VulkanObject<VulkanDescriptorGroup> {
public:
    VulkanDescriptorGroup();
    ~VulkanDescriptorGroup() override;
    
    bool Setup(Description description);
    VkDescriptorSet GetDescriptorSet() const;
    
private:
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 