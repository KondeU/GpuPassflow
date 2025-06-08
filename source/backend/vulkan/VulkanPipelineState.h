#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanPipelineState : public rhi::PipelineState
    , VulkanObject<VulkanPipelineState> {
public:
    VulkanPipelineState();
    ~VulkanPipelineState() override;
    
    bool Setup(Description description);
    VkPipeline GetPipeline() const;
    
private:
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 