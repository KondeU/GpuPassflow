#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanImageBuffer : public rhi::ImageBuffer
    , VulkanObject<VulkanImageBuffer> {
public:
    VulkanImageBuffer();
    ~VulkanImageBuffer() override;
    
    bool Setup(Description description);
    VkImage GetImage() const;
    VkImageView GetImageView() const;
    
private:
    VkImage image = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 