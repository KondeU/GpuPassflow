#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"
#include "VulkanMemoryAllocator.h"

namespace au::backend {

class VulkanDevice;

class VulkanVertexBuffer : public rhi::VertexBuffer
    , VulkanObject<VulkanVertexBuffer> {
public:
    explicit VulkanVertexBuffer(VulkanDevice& device);
    ~VulkanVertexBuffer() override;

    bool Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    VkBuffer GetBuffer() const;
    VkDeviceSize GetSize() const;
    VkDeviceSize GetStride() const;

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