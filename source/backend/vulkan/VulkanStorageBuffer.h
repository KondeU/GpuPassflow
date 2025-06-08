#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanStorageBuffer : public rhi::StorageBuffer
    , VulkanObject<VulkanStorageBuffer> {
public:
    VulkanStorageBuffer();
    ~VulkanStorageBuffer() override;
    
    bool Setup(Description description);
    VkBuffer GetBuffer() const;
    
private:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 