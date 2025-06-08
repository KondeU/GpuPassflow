#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanCommandRecorder : public rhi::CommandRecorder
    , VulkanObject<VulkanCommandRecorder> {
public:
    explicit VulkanCommandRecorder();
    ~VulkanCommandRecorder() override;

    bool Setup(Description description);
    void Shutdown();

    void Begin() override;
    void End() override;
    void Execute() override;

    VkCommandBuffer GetCommandBuffer() const;

private:
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 
 