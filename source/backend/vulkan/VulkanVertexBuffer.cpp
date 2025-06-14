#include "VulkanVertexBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommon.h"

namespace au::backend {

VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice& device) : vulkanDevice(device)
{
    this->device = device.NativeDevice();
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    Shutdown();
}

bool VulkanVertexBuffer::Setup(Description description)
{
    this->description = description;
    
    bufferSize = static_cast<VkDeviceSize>(description.verticesCount) * description.attributesByteSize;
    if (bufferSize == 0) {
        GP_LOG_F(TAG, "Create vertex buffer failed, buffer size is zero!");
        return false;
    }
    
    // Create buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    bool success = false;
    LogOutIfFailedE(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer), success);
    
    if (!success) {
        GP_LOG_E(TAG, "Failed to create vertex buffer");
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
        GP_LOG_E(TAG, "Failed to allocate vertex buffer memory");
        vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
        return false;
    }
    
    GP_LOG_I(TAG, "Created vertex buffer: vertices=%u, stride=%u, size=%llu, memory_type=%d",
        description.verticesCount, description.attributesByteSize, bufferSize, (int)description.memoryType);
    
    return true;
}

void VulkanVertexBuffer::Shutdown()
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

void* VulkanVertexBuffer::Map()
{
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        GP_LOG_W(TAG, "Trying to map GPU-only vertex buffer");
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
        GP_LOG_E(TAG, "Failed to map vertex buffer memory");
    }
    
    return mappedPtr;
}

void VulkanVertexBuffer::Unmap()
{
    if (!mappedPtr) {
        return;
    }
    
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        GP_LOG_W(TAG, "Trying to unmap GPU-only vertex buffer");
        return;
    }
    
    VulkanMemoryAllocator* allocator = vulkanDevice.GetMemoryAllocator();
    if (allocator) {
        allocator->UnmapMemory(allocation);
    }
    
    mappedPtr = nullptr;
}

VkBuffer VulkanVertexBuffer::GetBuffer() const
{
    return buffer;
}

VkDeviceSize VulkanVertexBuffer::GetSize() const
{
    return bufferSize;
}

VkDeviceSize VulkanVertexBuffer::GetStride() const
{
    return static_cast<VkDeviceSize>(description.attributesByteSize);
}

} 