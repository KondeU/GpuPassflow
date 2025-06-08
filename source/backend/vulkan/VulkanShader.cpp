#include "VulkanShader.h"

namespace au::backend {

VulkanShader::VulkanShader() {}
VulkanShader::~VulkanShader() { Shutdown(); }

bool VulkanShader::Setup(Description description) {
    // TODO: Implement shader creation
    return true;
}

void VulkanShader::Shutdown() {
    if (shaderModule != VK_NULL_HANDLE && device != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, shaderModule, nullptr);
        shaderModule = VK_NULL_HANDLE;
    }
}

VkShaderModule VulkanShader::GetShaderModule() const { return shaderModule; }
VkShaderStageFlagBits VulkanShader::GetStageFlags() const { return stageFlags; }

} 