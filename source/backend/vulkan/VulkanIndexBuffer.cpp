#include "VulkanIndexBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommon.h"

namespace au::backend {

VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice& device) : vulkanDevice(device)
{
    this->device = device.NativeDevice();
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
    Shutdown();
}

bool VulkanIndexBuffer::Setup(Description description)
{
    this->description = description;
    
    bufferSize = static_cast<VkDeviceSize>(description.indicesCount) * description.indexByteSize;
    if (bufferSize == 0) {
        GP_LOG_F(TAG, "Create index buffer failed, buffer size is zero!");
        return false;
    }
    
    // Create buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    bool success = false;
    LogOutIfFailedE(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer), success);
    
    if (!success) {
        GP_LOG_E(TAG, "Failed to create index buffer");
        return false;
    }
    
    // Allocate memory
    VulkanMemoryAllocator* allocator = vulkanDevice.GetMemoryAllocator();
    if (!allocator) {
        GP_LOG_E(TAG, "Memory allocator is null");
        vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
        return false;
    }
    
    if (!allocator->AllocateBufferMemory(buffer, description.memoryType, allocation)) {
        GP_LOG_E(TAG, "Failed to allocate index buffer memory");
        vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
        return false;
    }
    
    GP_LOG_I(TAG, "Created index buffer: indices=%u, index_size=%u, size=%llu, memory_type=%d",
        description.indicesCount, description.indexByteSize, bufferSize, (int)description.memoryType);
    
    return true;
}

void VulkanIndexBuffer::Shutdown()
{
    if (mappedPtr) {
        Unmap();
    }
    
    if (allocation.memory != VK_NULL_HANDLE) {
        VulkanMemoryAllocator* allocator = vulkanDevice.GetMemoryAllocator();
        if (allocator) {
            allocator->FreeMemory(allocation);
        }
    }
    
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    
    description = {0, 0, rhi::TransferDirection::GPU_ONLY};
    bufferSize = 0;
    mappedPtr = nullptr;
}

void* VulkanIndexBuffer::Map()
{
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        GP_LOG_W(TAG, "Trying to map GPU-only index buffer");
        return nullptr;
    }
    
    if (mappedPtr) {
        // Already mapped
        return mappedPtr;
    }
    
    VulkanMemoryAllocator* allocator = vulkanDevice.GetMemoryAllocator();
    if (!allocator) {
        GP_LOG_E(TAG, "Memory allocator is null");
        return nullptr;
    }
    
    mappedPtr = allocator->MapMemory(allocation);
    if (!mappedPtr) {
        GP_LOG_E(TAG, "Failed to map index buffer memory");
    }
    
    return mappedPtr;
}

void VulkanIndexBuffer::Unmap()
{
    if (!mappedPtr) {
        return;
    }
    
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        GP_LOG_W(TAG, "Trying to unmap GPU-only index buffer");
        return;
    }
    
    VulkanMemoryAllocator* allocator = vulkanDevice.GetMemoryAllocator();
    if (allocator) {
        allocator->UnmapMemory(allocation);
    }
    
    mappedPtr = nullptr;
}

VkBuffer VulkanIndexBuffer::GetBuffer() const
{
    return buffer;
}

VkDeviceSize VulkanIndexBuffer::GetSize() const
{
    return bufferSize;
}

VkIndexType VulkanIndexBuffer::GetIndexType() const
{
    switch (description.indexByteSize) {
        case 2: return VK_INDEX_TYPE_UINT16;
        case 4: return VK_INDEX_TYPE_UINT32;
        default: 
            GP_LOG_E(TAG, "Invalid index byte size: %u", description.indexByteSize);
            return VK_INDEX_TYPE_UINT16;
    }
}

} 