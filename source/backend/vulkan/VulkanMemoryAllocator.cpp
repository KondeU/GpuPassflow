#include "VulkanMemoryAllocator.h"
#include "VulkanDevice.h"
#include "VulkanCommon.h"

namespace au::backend {

VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanDevice* device) : device(device)
{
}

VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
    Shutdown();
}

bool VulkanMemoryAllocator::Setup()
{
    if (!device) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Device is null");
        return false;
    }
    
    vkDevice = device->NativeDevice();
    physicalDevice = device->PhysicalDevice();
    
    if (vkDevice == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Invalid device handles");
        return false;
    }
    
    // Get memory properties
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    
    GP_LOG_I(TAG, "VulkanMemoryAllocator: Setup complete. Memory types: %u, Memory heaps: %u", 
        memoryProperties.memoryTypeCount, memoryProperties.memoryHeapCount);
    
    return true;
}

void VulkanMemoryAllocator::Shutdown()
{
    // Memory allocator doesn't hold any persistent resources
    vkDevice = VK_NULL_HANDLE;
    physicalDevice = VK_NULL_HANDLE;
    memoryProperties = {};
}

bool VulkanMemoryAllocator::AllocateBufferMemory(VkBuffer buffer, rhi::TransferDirection memoryType, VulkanMemoryAllocation& allocation)
{
    if (buffer == VK_NULL_HANDLE) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Invalid buffer handle");
        return false;
    }
    
    // Get buffer memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkDevice, buffer, &memRequirements);
    
    // Find suitable memory type
    VkMemoryPropertyFlags properties = ConvertMemoryType(memoryType);
    uint32_t memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
    
    if (memoryTypeIndex == UINT32_MAX) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to find suitable memory type for buffer");
        return false;
    }
    
    // Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    bool success = false;
    LogOutIfFailedE(vkAllocateMemory(vkDevice, &allocInfo, nullptr, &allocation.memory), success);
    
    if (!success) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to allocate buffer memory");
        return false;
    }
    
    allocation.offset = 0;
    allocation.size = memRequirements.size;
    allocation.memoryTypeIndex = memoryTypeIndex;
    allocation.mappedPtr = nullptr;
    
    // Bind buffer to memory
    LogOutIfFailedE(vkBindBufferMemory(vkDevice, buffer, allocation.memory, allocation.offset), success);
    
    if (!success) {
        vkFreeMemory(vkDevice, allocation.memory, nullptr);
        allocation.memory = VK_NULL_HANDLE;
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to bind buffer memory");
        return false;
    }
    
    return true;
}

bool VulkanMemoryAllocator::AllocateImageMemory(VkImage image, rhi::TransferDirection memoryType, VulkanMemoryAllocation& allocation)
{
    if (image == VK_NULL_HANDLE) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Invalid image handle");
        return false;
    }
    
    // Get image memory requirements
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice, image, &memRequirements);
    
    // Find suitable memory type
    VkMemoryPropertyFlags properties = ConvertMemoryType(memoryType);
    uint32_t memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
    
    if (memoryTypeIndex == UINT32_MAX) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to find suitable memory type for image");
        return false;
    }
    
    // Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    bool success = false;
    LogOutIfFailedE(vkAllocateMemory(vkDevice, &allocInfo, nullptr, &allocation.memory), success);
    
    if (!success) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to allocate image memory");
        return false;
    }
    
    allocation.offset = 0;
    allocation.size = memRequirements.size;
    allocation.memoryTypeIndex = memoryTypeIndex;
    allocation.mappedPtr = nullptr;
    
    // Bind image to memory
    LogOutIfFailedE(vkBindImageMemory(vkDevice, image, allocation.memory, allocation.offset), success);
    
    if (!success) {
        vkFreeMemory(vkDevice, allocation.memory, nullptr);
        allocation.memory = VK_NULL_HANDLE;
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to bind image memory");
        return false;
    }
    
    return true;
}

void* VulkanMemoryAllocator::MapMemory(const VulkanMemoryAllocation& allocation)
{
    if (allocation.memory == VK_NULL_HANDLE) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Invalid memory handle for mapping");
        return nullptr;
    }
    
    if (allocation.mappedPtr != nullptr) {
        // Already mapped
        return allocation.mappedPtr;
    }
    
    void* mappedPtr = nullptr;
    bool success = false;
    LogOutIfFailedE(vkMapMemory(vkDevice, allocation.memory, allocation.offset, allocation.size, 0, &mappedPtr), success);
    
    if (!success) {
        GP_LOG_E(TAG, "VulkanMemoryAllocator: Failed to map memory");
        return nullptr;
    }
    
    return mappedPtr;
}

void VulkanMemoryAllocator::UnmapMemory(const VulkanMemoryAllocation& allocation)
{
    if (allocation.memory == VK_NULL_HANDLE) {
        GP_LOG_W(TAG, "VulkanMemoryAllocator: Trying to unmap invalid memory handle");
        return;
    }
    
    vkUnmapMemory(vkDevice, allocation.memory);
}

void VulkanMemoryAllocator::FreeMemory(VulkanMemoryAllocation& allocation)
{
    if (allocation.memory != VK_NULL_HANDLE) {
        if (allocation.mappedPtr != nullptr) {
            UnmapMemory(allocation);
        }
        vkFreeMemory(vkDevice, allocation.memory, nullptr);
        allocation.memory = VK_NULL_HANDLE;
    }
    
    allocation.offset = 0;
    allocation.size = 0;
    allocation.mappedPtr = nullptr;
    allocation.memoryTypeIndex = UINT32_MAX;
}

uint32_t VulkanMemoryAllocator::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    return UINT32_MAX;
}

VkMemoryPropertyFlags VulkanMemoryAllocator::ConvertMemoryType(rhi::TransferDirection type)
{
    switch (type) {
        case rhi::TransferDirection::CPU_TO_GPU:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        case rhi::TransferDirection::GPU_TO_CPU:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        case rhi::TransferDirection::GPU_ONLY:
        default:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
}

} 