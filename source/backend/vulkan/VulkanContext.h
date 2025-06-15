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
    void CreateInstance();
    void SetupDebugMessenger();
    void EnumeratePhysicalDevices();
    void DestroyDebugMessenger();

    std::vector<const char*> GetRequiredInstanceExtensions() const;
    std::vector<const char*> GetRequiredValidationLayers() const;
    bool CheckValidationLayerSupport() const;
    bool IsDeviceSuitable(VkPhysicalDevice device) const;

    VkInstance vkinst;
    InstanceContainer<VulkanDevice> devices;
    VkDebugUtilsMessengerEXT debugMessenger;
    std::vector<VkPhysicalDevice> physicalDevices;
};

}
