#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanPipelineLayout : public rhi::PipelineLayout
    , VulkanObject<VulkanPipelineLayout> {
public:
    VulkanPipelineLayout();
    ~VulkanPipelineLayout() override;
    
    bool Setup(Description description);
    VkPipelineLayout GetPipelineLayout() const;
    
private:
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
};

} 