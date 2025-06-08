#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanShader : public rhi::Shader
    , VulkanObject<VulkanShader> {
public:
    explicit VulkanShader();
    ~VulkanShader() override;

    bool Setup(Description description);
    void Shutdown();

    VkShaderModule GetShaderModule() const;
    VkShaderStageFlagBits GetStageFlags() const;

private:
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VkShaderStageFlagBits stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    VkDevice device = VK_NULL_HANDLE;
};

} 