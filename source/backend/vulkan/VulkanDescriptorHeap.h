#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanDescriptorHeap : public rhi::DescriptorHeap
    , VulkanObject<VulkanDescriptorHeap> {
public:
    VulkanDescriptorHeap();
    ~VulkanDescriptorHeap() override;
    
    bool Setup(Description description);
    VkDescriptorPool GetDescriptorPool() const;
    
private:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 