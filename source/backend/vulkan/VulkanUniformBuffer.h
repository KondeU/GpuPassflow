#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanUniformBuffer : public rhi::UniformBuffer
    , VulkanObject<VulkanUniformBuffer> {
public:
    VulkanUniformBuffer();
    ~VulkanUniformBuffer() override;
    
    bool Setup(Description description);
    VkBuffer GetBuffer() const;
    
private:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 