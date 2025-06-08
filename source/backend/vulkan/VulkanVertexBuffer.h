#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanVertexBuffer : public rhi::VertexBuffer
    , VulkanObject<VulkanVertexBuffer> {
public:
    explicit VulkanVertexBuffer();
    ~VulkanVertexBuffer() override;

    bool Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    VkBuffer GetBuffer() const;
    VkDeviceMemory GetMemory() const;

private:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 