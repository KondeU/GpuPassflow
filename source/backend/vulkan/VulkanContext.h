#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanDevice.h"

namespace au::backend {

class VulkanContext : public rhi::BackendContext
    , VulkanObject<VulkanContext> {
public:
    explicit VulkanContext();
    ~VulkanContext() override;

    rhi::Device* CreateDevice(rhi::Device::Description description) override;
    bool DestroyDevice(rhi::Device* device) override;

    std::vector<std::string> GetAvailableAdaptors() const override;

private:
    VkInstance instance = VK_NULL_HANDLE;
    InstanceContainer<VulkanDevice> devices;
    
    bool SetupInstance();
    void EnumeratePhysicalDevices();
    
    std::vector<VkPhysicalDevice> physicalDevices;
};

} 