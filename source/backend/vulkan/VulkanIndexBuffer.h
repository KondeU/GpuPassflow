#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"
#include "VulkanMemoryAllocator.h"

namespace au::backend {

class VulkanDevice;

class VulkanIndexBuffer : public rhi::IndexBuffer
    , VulkanObject<VulkanIndexBuffer> {
public:
    explicit VulkanIndexBuffer(VulkanDevice& device);
    ~VulkanIndexBuffer() override;
    
    bool Setup(Description description);
    void Shutdown();
    
    void* Map() override;
    void Unmap() override;
    
    VkBuffer GetBuffer() const;
    VkDeviceSize GetSize() const;
    VkIndexType GetIndexType() const;
    
private:
    VulkanDevice& vulkanDevice;
    VkDevice device = VK_NULL_HANDLE;
    VkBuffer buffer = VK_NULL_HANDLE;
    VulkanMemoryAllocation allocation = {};
    
    Description description = {0, 0, rhi::TransferDirection::GPU_ONLY};
    VkDeviceSize bufferSize = 0;
    void* mappedPtr = nullptr;
};

} 