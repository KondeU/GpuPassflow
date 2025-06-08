#pragma once

#include <set>
#include "VulkanShader.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandRecorder.h"
#include "VulkanVertexBuffer.h"
#include "VulkanVertexAttribute.h"
#include "VulkanIndexBuffer.h"
#include "VulkanIndexAttribute.h"
#include "VulkanConstantBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanStorageBuffer.h"
#include "VulkanImageBuffer.h"
#include "VulkanImageSampler.h"
#include "VulkanDescriptorHeap.h"
#include "VulkanDescriptorGroup.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipelineState.h"

namespace au::backend {

class VulkanDevice : public rhi::Device
    , VulkanObject<VulkanDevice> {
public:
    explicit VulkanDevice(VkInstance instance);
    ~VulkanDevice() override;

    bool Setup(Description description);
    void Shutdown();

    rhi::Shader* CreateShader(
        rhi::Shader::Description description) override;
    bool DestroyShader(rhi::Shader* instance) override;

    rhi::Swapchain* CreateSwapchain(
        rhi::Swapchain::Description description) override;
    bool DestroySwapchain(rhi::Swapchain* instance) override;

    rhi::CommandRecorder* CreateCommandRecorder(
        rhi::CommandRecorder::Description description) override;
    bool DestroyCommandRecorder(rhi::CommandRecorder* instance) override;

    rhi::VertexBuffer* CreateVertexBuffer(
        rhi::VertexBuffer::Description description) override;
    bool DestroyVertexBuffer(rhi::VertexBuffer* instance) override;

    rhi::VertexAttribute* CreateVertexAttribute() override;
    bool DestroyVertexAttribute(rhi::VertexAttribute* instance) override;

    rhi::ConstantBuffer* CreateConstantBuffer() override;
    bool DestroyConstantBuffer(rhi::ConstantBuffer* instance) override;

    rhi::IndexBuffer* CreateIndexBuffer(
        rhi::IndexBuffer::Description description) override;
    bool DestroyIndexBuffer(rhi::IndexBuffer* instance) override;

    rhi::IndexAttribute* CreateIndexAttribute() override;
    bool DestroyIndexAttribute(rhi::IndexAttribute* instance) override;

    rhi::UniformBuffer* CreateUniformBuffer(
        rhi::UniformBuffer::Description description) override;
    bool DestroyUniformBuffer(rhi::UniformBuffer* instance) override;

    rhi::StorageBuffer* CreateStorageBuffer(
        rhi::StorageBuffer::Description description) override;
    bool DestroyResourceBuffer(rhi::StorageBuffer* instance) override;

    rhi::ImageBuffer* CreateImageBuffer(
        rhi::ImageBuffer::Description description) override;
    bool DestroyImageBuffer(rhi::ImageBuffer* instance) override;

    rhi::ImageSampler* CreateImageSampler(
        rhi::ImageSampler::Description description) override;
    bool DestroyImageSampler(rhi::ImageSampler* instance) override;

    rhi::DescriptorHeap* CreateDescriptorHeap(
        rhi::DescriptorHeap::Description description) override;
    bool DestroyDescriptorHeap(rhi::DescriptorHeap* instance) override;

    rhi::DescriptorGroup* CreateDescriptorGroup(
        rhi::DescriptorGroup::Description description) override;
    bool DestroyDescriptorGroup(rhi::DescriptorGroup* instance) override;

    rhi::PipelineLayout* CreatePipelineLayout(
        rhi::PipelineLayout::Description description) override;
    bool DestroyPipelineLayout(rhi::PipelineLayout* instance) override;

    rhi::PipelineState* CreatePipelineState(
        rhi::PipelineState::Description description) override;
    bool DestroyPipelineState(rhi::PipelineState* instance) override;

    void WaitIdle() override;

    void ReleaseCommandRecordersMemory(const std::string& commandContainer) override;

    VkInstance VulkanInstance();
    VkPhysicalDevice PhysicalDevice();
    VkDevice NativeDevice();
    VkQueue GraphicsQueue();
    VkQueue PresentQueue();
    VkQueue ComputeQueue();
    VkCommandPool CommandPool(const std::string& name);
    uint32_t GraphicsQueueFamily() const;
    uint32_t PresentQueueFamily() const;
    uint32_t ComputeQueueFamily() const;

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    
    Description description;
    
    // Queue families
    uint32_t graphicsQueueFamily = UINT32_MAX;
    uint32_t presentQueueFamily = UINT32_MAX;
    uint32_t computeQueueFamily = UINT32_MAX;
    
    // Queues
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkQueue computeQueue = VK_NULL_HANDLE;
    
    // Command pools
    std::unordered_map<std::string, VkCommandPool> commandPools;

    InstanceContainer<VulkanShader> shaders;
    InstanceContainer<VulkanSwapchain> swapchains;
    InstanceContainer<VulkanCommandRecorder> commandRecorders;
    InstanceContainer<VulkanIndexBuffer> indexBuffers;
    InstanceContainer<VulkanVertexBuffer> vertexBuffers;
    InstanceContainer<VulkanIndexAttribute> indexAttributes;
    InstanceContainer<VulkanVertexAttribute> vertexAttributes;
    InstanceContainer<VulkanConstantBuffer> constantBuffers;
    InstanceContainer<VulkanUniformBuffer> uniformBuffers;
    InstanceContainer<VulkanStorageBuffer> storageBuffers;
    InstanceContainer<VulkanImageBuffer> imageBuffers;
    InstanceContainer<VulkanImageSampler> imageSamplers;
    InstanceContainer<VulkanDescriptorHeap> descriptorHeaps;
    InstanceContainer<VulkanDescriptorGroup> descriptorGroups;
    InstanceContainer<VulkanPipelineLayout> pipelineLayouts;
    InstanceContainer<VulkanPipelineState> pipelineStates;
    
    bool SetupPhysicalDevice();
    bool SetupLogicalDevice();
    bool SetupQueues();
};

} 