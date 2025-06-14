#include "VulkanDevice.h"
#include "VulkanCommon.h"
#include <set>

namespace au::backend {

VulkanDevice::VulkanDevice(VkInstance instance) : instance(instance)
{
}

VulkanDevice::~VulkanDevice()
{
    Shutdown();
}

bool VulkanDevice::Setup(Description description)
{
    this->description = description;
    
    if (!SetupPhysicalDevice()) return false;
    if (!SetupLogicalDevice()) return false;
    if (!SetupQueues()) return false;
    
    // Setup memory allocator
    memoryAllocator = std::make_unique<VulkanMemoryAllocator>(this);
    if (!memoryAllocator->Setup()) {
        GP_LOG_E(TAG, "Failed to setup memory allocator");
        return false;
    }
    
    return true;
}

void VulkanDevice::Shutdown()
{
    // Clean up memory allocator first
    if (memoryAllocator) {
        memoryAllocator->Shutdown();
        memoryAllocator.reset();
    }
    
    // Clean up command pools
    for (auto& pair : commandPools) {
        vkDestroyCommandPool(device, pair.second, nullptr);
    }
    commandPools.clear();
    
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
}

// Stub implementations for all interface methods
rhi::Shader* VulkanDevice::CreateShader(rhi::Shader::Description description)
{
    return CreateInstance<rhi::Shader, VulkanShader>(shaders, description);
}

bool VulkanDevice::DestroyShader(rhi::Shader* instance)
{
    return DestroyInstance<rhi::Shader, VulkanShader>(shaders, instance);
}

rhi::Swapchain* VulkanDevice::CreateSwapchain(rhi::Swapchain::Description description)
{
    return CreateInstance<rhi::Swapchain, VulkanSwapchain>(swapchains, description);
}

bool VulkanDevice::DestroySwapchain(rhi::Swapchain* instance)
{
    return DestroyInstance<rhi::Swapchain, VulkanSwapchain>(swapchains, instance);
}

rhi::CommandRecorder* VulkanDevice::CreateCommandRecorder(rhi::CommandRecorder::Description description)
{
    return CreateInstance<rhi::CommandRecorder, VulkanCommandRecorder>(commandRecorders, description);
}

bool VulkanDevice::DestroyCommandRecorder(rhi::CommandRecorder* instance)
{
    return DestroyInstance<rhi::CommandRecorder, VulkanCommandRecorder>(commandRecorders, instance);
}

rhi::VertexBuffer* VulkanDevice::CreateVertexBuffer(rhi::VertexBuffer::Description description)
{
    auto vertexBuffer = std::make_unique<VulkanVertexBuffer>(*this);
    if (!vertexBuffer->Setup(description)) {
        return nullptr;
    }
    auto instance = vertexBuffer.get();
    vertexBuffers[instance] = std::move(vertexBuffer);
    return instance;
}

bool VulkanDevice::DestroyVertexBuffer(rhi::VertexBuffer* instance)
{
    return DestroyInstance<rhi::VertexBuffer, VulkanVertexBuffer>(vertexBuffers, instance);
}

rhi::VertexAttribute* VulkanDevice::CreateVertexAttribute()
{
    rhi::VertexAttribute::Description desc = {};
    return CreateInstance<rhi::VertexAttribute, VulkanVertexAttribute>(vertexAttributes, desc);
}

bool VulkanDevice::DestroyVertexAttribute(rhi::VertexAttribute* instance)
{
    return DestroyInstance<rhi::VertexAttribute, VulkanVertexAttribute>(vertexAttributes, instance);
}

rhi::ConstantBuffer* VulkanDevice::CreateConstantBuffer()
{
    rhi::ConstantBuffer::Description desc = {};
    return CreateInstance<rhi::ConstantBuffer, VulkanConstantBuffer>(constantBuffers, desc);
}

bool VulkanDevice::DestroyConstantBuffer(rhi::ConstantBuffer* instance)
{
    return DestroyInstance<rhi::ConstantBuffer, VulkanConstantBuffer>(constantBuffers, instance);
}

rhi::IndexBuffer* VulkanDevice::CreateIndexBuffer(rhi::IndexBuffer::Description description)
{
    auto indexBuffer = std::make_unique<VulkanIndexBuffer>(*this);
    if (!indexBuffer->Setup(description)) {
        return nullptr;
    }
    auto instance = indexBuffer.get();
    indexBuffers[instance] = std::move(indexBuffer);
    return instance;
}

bool VulkanDevice::DestroyIndexBuffer(rhi::IndexBuffer* instance)
{
    return DestroyInstance<rhi::IndexBuffer, VulkanIndexBuffer>(indexBuffers, instance);
}

rhi::IndexAttribute* VulkanDevice::CreateIndexAttribute()
{
    rhi::IndexAttribute::Description desc = {};
    return CreateInstance<rhi::IndexAttribute, VulkanIndexAttribute>(indexAttributes, desc);
}

bool VulkanDevice::DestroyIndexAttribute(rhi::IndexAttribute* instance)
{
    return DestroyInstance<rhi::IndexAttribute, VulkanIndexAttribute>(indexAttributes, instance);
}

rhi::UniformBuffer* VulkanDevice::CreateUniformBuffer(rhi::UniformBuffer::Description description)
{
    return CreateInstance<rhi::UniformBuffer, VulkanUniformBuffer>(uniformBuffers, description);
}

bool VulkanDevice::DestroyUniformBuffer(rhi::UniformBuffer* instance)
{
    return DestroyInstance<rhi::UniformBuffer, VulkanUniformBuffer>(uniformBuffers, instance);
}

rhi::StorageBuffer* VulkanDevice::CreateStorageBuffer(rhi::StorageBuffer::Description description)
{
    return CreateInstance<rhi::StorageBuffer, VulkanStorageBuffer>(storageBuffers, description);
}

bool VulkanDevice::DestroyResourceBuffer(rhi::StorageBuffer* instance)
{
    return DestroyInstance<rhi::StorageBuffer, VulkanStorageBuffer>(storageBuffers, instance);
}

rhi::ImageBuffer* VulkanDevice::CreateImageBuffer(rhi::ImageBuffer::Description description)
{
    return CreateInstance<rhi::ImageBuffer, VulkanImageBuffer>(imageBuffers, description);
}

bool VulkanDevice::DestroyImageBuffer(rhi::ImageBuffer* instance)
{
    return DestroyInstance<rhi::ImageBuffer, VulkanImageBuffer>(imageBuffers, instance);
}

rhi::ImageSampler* VulkanDevice::CreateImageSampler(rhi::ImageSampler::Description description)
{
    return CreateInstance<rhi::ImageSampler, VulkanImageSampler>(imageSamplers, description);
}

bool VulkanDevice::DestroyImageSampler(rhi::ImageSampler* instance)
{
    return DestroyInstance<rhi::ImageSampler, VulkanImageSampler>(imageSamplers, instance);
}

rhi::DescriptorHeap* VulkanDevice::CreateDescriptorHeap(rhi::DescriptorHeap::Description description)
{
    return CreateInstance<rhi::DescriptorHeap, VulkanDescriptorHeap>(descriptorHeaps, description);
}

bool VulkanDevice::DestroyDescriptorHeap(rhi::DescriptorHeap* instance)
{
    return DestroyInstance<rhi::DescriptorHeap, VulkanDescriptorHeap>(descriptorHeaps, instance);
}

rhi::DescriptorGroup* VulkanDevice::CreateDescriptorGroup(rhi::DescriptorGroup::Description description)
{
    return CreateInstance<rhi::DescriptorGroup, VulkanDescriptorGroup>(descriptorGroups, description);
}

bool VulkanDevice::DestroyDescriptorGroup(rhi::DescriptorGroup* instance)
{
    return DestroyInstance<rhi::DescriptorGroup, VulkanDescriptorGroup>(descriptorGroups, instance);
}

rhi::PipelineLayout* VulkanDevice::CreatePipelineLayout(rhi::PipelineLayout::Description description)
{
    return CreateInstance<rhi::PipelineLayout, VulkanPipelineLayout>(pipelineLayouts, description);
}

bool VulkanDevice::DestroyPipelineLayout(rhi::PipelineLayout* instance)
{
    return DestroyInstance<rhi::PipelineLayout, VulkanPipelineLayout>(pipelineLayouts, instance);
}

rhi::PipelineState* VulkanDevice::CreatePipelineState(rhi::PipelineState::Description description)
{
    return CreateInstance<rhi::PipelineState, VulkanPipelineState>(pipelineStates, description);
}

bool VulkanDevice::DestroyPipelineState(rhi::PipelineState* instance)
{
    return DestroyInstance<rhi::PipelineState, VulkanPipelineState>(pipelineStates, instance);
}

void VulkanDevice::WaitIdle()
{
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
    }
}

void VulkanDevice::ReleaseCommandRecordersMemory(const std::string& commandContainer)
{
    // TODO: Implement command recorder memory release
}

// Accessor methods
VkInstance VulkanDevice::VulkanInstance() { return instance; }
VkPhysicalDevice VulkanDevice::PhysicalDevice() { return physicalDevice; }
VkDevice VulkanDevice::NativeDevice() { return device; }
VkQueue VulkanDevice::GraphicsQueue() { return graphicsQueue; }
VkQueue VulkanDevice::PresentQueue() { return presentQueue; }
VkQueue VulkanDevice::ComputeQueue() { return computeQueue; }
uint32_t VulkanDevice::GraphicsQueueFamily() const { return graphicsQueueFamily; }
uint32_t VulkanDevice::PresentQueueFamily() const { return presentQueueFamily; }
uint32_t VulkanDevice::ComputeQueueFamily() const { return computeQueueFamily; }

VulkanMemoryAllocator* VulkanDevice::GetMemoryAllocator() { return memoryAllocator.get(); }

VkCommandPool VulkanDevice::CommandPool(const std::string& name)
{
    auto it = commandPools.find(name);
    if (it != commandPools.end()) {
        return it->second;
    }
    
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    
    VkCommandPool commandPool;
    bool success = false;
    LogOutIfFailedE(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool), success);
    
    if (success) {
        commandPools[name] = commandPool;
        return commandPool;
    }
    
    return VK_NULL_HANDLE;
}

bool VulkanDevice::SetupPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        GP_LOG_E(TAG, "Failed to find GPUs with Vulkan support!");
        return false;
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    
    // For now, just pick the first device
    physicalDevice = devices[0];
    return true;
}

bool VulkanDevice::SetupLogicalDevice()
{
    // Find queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
    
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamily = i;
            presentQueueFamily = i; // Assume same for now
        }
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            computeQueueFamily = i;
        }
    }
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {graphicsQueueFamily, presentQueueFamily, computeQueueFamily};
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    bool success = false;
    LogOutIfFailedE(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), success);
    return success;
}

bool VulkanDevice::SetupQueues()
{
    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);
    vkGetDeviceQueue(device, computeQueueFamily, 0, &computeQueue);
    return true;
}

} 