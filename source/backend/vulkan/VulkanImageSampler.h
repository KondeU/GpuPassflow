#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanImageSampler : public rhi::ImageSampler
    , VulkanObject<VulkanImageSampler> {
public:
    VulkanImageSampler();
    ~VulkanImageSampler() override;
    
    bool Setup(Description description);
    VkSampler GetSampler() const;
    
private:
    VkSampler sampler = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 