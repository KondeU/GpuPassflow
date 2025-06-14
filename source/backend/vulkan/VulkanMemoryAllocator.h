#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanDevice;

struct VulkanMemoryAllocation {
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize offset = 0;
    VkDeviceSize size = 0;
    void* mappedPtr = nullptr;
    uint32_t memoryTypeIndex = UINT32_MAX;
};

class VulkanMemoryAllocator : VulkanObject<VulkanMemoryAllocator> {
public:
    explicit VulkanMemoryAllocator(VulkanDevice* device);
    ~VulkanMemoryAllocator();

    bool Setup();
    void Shutdown();

    // Buffer allocation
    bool AllocateBufferMemory(VkBuffer buffer, rhi::TransferDirection memoryType, VulkanMemoryAllocation& allocation);
    
    // Image allocation
    bool AllocateImageMemory(VkImage image, rhi::TransferDirection memoryType, VulkanMemoryAllocation& allocation);
    
    // Memory operations
    void* MapMemory(const VulkanMemoryAllocation& allocation);
    void UnmapMemory(const VulkanMemoryAllocation& allocation);
    void FreeMemory(VulkanMemoryAllocation& allocation);

private:
    VulkanDevice* device;
    VkDevice vkDevice = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkMemoryPropertyFlags ConvertMemoryType(rhi::TransferDirection type);
};

} 