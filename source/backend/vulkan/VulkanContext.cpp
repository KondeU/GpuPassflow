#include "VulkanContext.h"
#include "VulkanCommon.h"

namespace au::backend {

VulkanContext::VulkanContext()
{
    SetupInstance();
    EnumeratePhysicalDevices();
}

VulkanContext::~VulkanContext()
{
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
    }
}

rhi::Device* VulkanContext::CreateDevice(rhi::Device::Description description)
{
    return CreateInstance<rhi::Device, VulkanDevice>(devices, description, instance);
}

bool VulkanContext::DestroyDevice(rhi::Device* device)
{
    return DestroyInstance<rhi::Device, VulkanDevice>(devices, device);
}

std::vector<std::string> VulkanContext::GetAvailableAdaptors() const
{
    std::vector<std::string> adaptors;
    
    for (const auto& physicalDevice : physicalDevices) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        adaptors.push_back(properties.deviceName);
    }
    
    return adaptors;
}

bool VulkanContext::SetupInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GpuPassflow";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "GpuPassflow Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Enable required extensions
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    bool success = false;
    LogOutIfFailedE(vkCreateInstance(&createInfo, nullptr, &instance), success);
    return success;
}

void VulkanContext::EnumeratePhysicalDevices()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    
    if (deviceCount > 0) {
        physicalDevices.resize(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
    }
}

} 