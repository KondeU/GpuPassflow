#include "VulkanContext.h"
#include "VulkanCommon.h"
#include <set>
#include <cstring>

namespace au::backend {

GP_LOG_TAG(VulkanContext);

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        GP_LOG_W(TAG, "Validation layer: %s", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

VulkanContext::VulkanContext() : vkinst(VK_NULL_HANDLE), debugMessenger(VK_NULL_HANDLE)
{
    GP_LOG_I(TAG, "Initializing Vulkan context...");
    CreateInstance();
    SetupDebugMessenger();
    EnumeratePhysicalDevices();
}

VulkanContext::~VulkanContext()
{
    devices.clear();

    DestroyDebugMessenger();

    if (vkinst != VK_NULL_HANDLE) {
        vkDestroyInstance(vkinst, nullptr);
        vkinst = VK_NULL_HANDLE;
    }

    GP_LOG_I(TAG, "Vulkan context destroyed.");
}

void VulkanContext::CreateInstance()
{
    if (!CheckValidationLayerSupport()) {
        GP_LOG_W(TAG, "Validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GPUPassflow Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "GPUPassflow Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredInstanceExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    auto validationLayers = GetRequiredValidationLayers();
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#if defined(DEBUG) || defined(_DEBUG)
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = DebugCallback;
    createInfo.pNext = &debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
#endif

    VkResult result = vkCreateInstance(&createInfo, nullptr, &vkinst);
    if (result != VK_SUCCESS) {
        GP_LOG_F(TAG, "Failed to create Vulkan instance: %s", FormatResult(result).c_str());
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    GP_LOG_I(TAG, "Vulkan instance created successfully.");
}

void VulkanContext::SetupDebugMessenger()
{
#if defined(DEBUG) || defined(_DEBUG)
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(vkinst, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        if (func(vkinst, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            GP_LOG_E(TAG, "Failed to set up debug messenger!");
        } else {
            GP_LOG_I(TAG, "Debug messenger set up successfully.");
        }
    } else {
        GP_LOG_W(TAG, "vkCreateDebugUtilsMessengerEXT function not found!");
    }
#endif
}

void VulkanContext::DestroyDebugMessenger()
{
#if defined(DEBUG) || defined(_DEBUG)
    if (debugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(vkinst, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(vkinst, debugMessenger, nullptr);
        }
        debugMessenger = VK_NULL_HANDLE;
    }
#endif
}

void VulkanContext::EnumeratePhysicalDevices()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkinst, &deviceCount, nullptr);

    if (deviceCount == 0) {
        GP_LOG_F(TAG, "Failed to find GPUs with Vulkan support!");
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    physicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(vkinst, &deviceCount, physicalDevices.data());

    GP_LOG_I(TAG, "Found %u Vulkan-capable devices.", deviceCount);
}

std::vector<std::string> VulkanContext::GetAvailableAdaptors() const
{
    std::vector<std::string> adaptorsList;

    GP_LOG_D(TAG, "Enumerating Vulkan physical devices...");

    for (size_t i = 0; i < physicalDevices.size(); i++) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

        GP_LOG_D(TAG, "> %d : %s (Type: %d, API Version: %u.%u.%u)",
                 static_cast<int>(i),
                 deviceProperties.deviceName,
                 deviceProperties.deviceType,
                 VK_VERSION_MAJOR(deviceProperties.apiVersion),
                 VK_VERSION_MINOR(deviceProperties.apiVersion),
                 VK_VERSION_PATCH(deviceProperties.apiVersion));

        adaptorsList.emplace_back(deviceProperties.deviceName);

        // Log memory information
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memProperties);

        GP_LOG_D(TAG, "  - Memory Types: %u, Memory Heaps: %u",
                 memProperties.memoryTypeCount, memProperties.memoryHeapCount);

        for (uint32_t j = 0; j < memProperties.memoryHeapCount; j++) {
            VkDeviceSize heapSize = memProperties.memoryHeaps[j].size;
            GP_LOG_D(TAG, "    Heap %u: %llu MB", j, heapSize / (1024 * 1024));
        }

        // Log queue families
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilies.data());

        GP_LOG_D(TAG, "  - Queue Families: %u", queueFamilyCount);
        for (uint32_t j = 0; j < queueFamilyCount; j++) {
            GP_LOG_D(TAG, "    Family %u: Count=%u, Flags=0x%x",
                     j, queueFamilies[j].queueCount, queueFamilies[j].queueFlags);
        }
    }

    return adaptorsList;
}

std::vector<const char*> VulkanContext::GetRequiredInstanceExtensions() const
{
    std::vector<const char*> extensions;

#if defined(DEBUG) || defined(_DEBUG)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    // Add platform-specific surface extensions
#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    return extensions;
}

std::vector<const char*> VulkanContext::GetRequiredValidationLayers() const
{
    std::vector<const char*> layers;

#if defined(DEBUG) || defined(_DEBUG)
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    return layers;
}

bool VulkanContext::CheckValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    auto validationLayers = GetRequiredValidationLayers();
    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device) const
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Check for required queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    bool hasGraphicsQueue = false;
    bool hasComputeQueue = false;

    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            hasGraphicsQueue = true;
        }
        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            hasComputeQueue = true;
        }
    }

    return hasGraphicsQueue && hasComputeQueue;
}

rhi::Device* VulkanContext::CreateDevice(rhi::Device::Description description)
{
    return CreateInstance<VulkanDevice>(devices, description, instance, physicalDevices);
}

bool VulkanContext::DestroyDevice(rhi::Device* device)
{
    return DestroyInstance(devices, device);
}

}
