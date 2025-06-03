#pragma once

#include "DX12Shader.h"
#include "DX12Swapchain.h"
#include "DX12CommandRecorder.h"
#include "DX12VertexBuffer.h"
#include "DX12VertexAttribute.h"
#include "DX12IndexBuffer.h"
#include "DX12IndexAttribute.h"
#include "DX12ConstantBuffer.h"
#include "DX12UniformBuffer.h"
#include "DX12StorageBuffer.h"
#include "DX12ImageBuffer.h"
#include "DX12ImageSampler.h"
#include "DX12DescriptorHeap.h"
#include "DX12DescriptorGroup.h"
#include "DX12PipelineLayout.h"
#include "DX12PipelineState.h"

namespace au::backend {

class DX12Device : public rhi::Device
    , DX12Object<DX12Device> {
public:
    explicit DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi);
    ~DX12Device() override;

    void Setup(Description description);
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

    Microsoft::WRL::ComPtr<IDXGIFactory4> DXGIFactory();
    Microsoft::WRL::ComPtr<ID3D12Device> NativeDevice();
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue(rhi::CommandType type);
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator(const std::string& name);

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;

    Description description;
    IDXGIAdapter* adapter = NULL;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    // TODO:  CommandQueue has not been abstracted into a separate class yet.
    std::unordered_map<rhi::CommandType, Microsoft::WRL::ComPtr<ID3D12CommandQueue>> queues;
    std::unordered_map<rhi::CommandType, std::pair<Microsoft::WRL::ComPtr<ID3D12Fence>, UINT64>> fences;

    // TODO: CommandMemory has not been abstracted into a separate class yet.
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> allocators;

    InstanceContainer<DX12Shader> shaders;
    InstanceContainer<DX12Swapchain> swapchains;
    InstanceContainer<DX12CommandRecorder> commandRecorders;
    InstanceContainer<DX12IndexBuffer> indexBuffers;
    InstanceContainer<DX12VertexBuffer> vertexBuffers;
    InstanceContainer<DX12IndexAttribute> indexAttributes;
    InstanceContainer<DX12VertexAttribute> vertexAttributes;
    InstanceContainer<DX12ConstantBuffer> constantBuffers;
    InstanceContainer<DX12UniformBuffer> uniformBuffers;
    InstanceContainer<DX12StorageBuffer> storageBuffers;
    InstanceContainer<DX12ImageBuffer> imageBuffers;
    InstanceContainer<DX12ImageSampler> imageSamplers;
    InstanceContainer<DX12DescriptorHeap> descriptorHeaps;
    InstanceContainer<DX12DescriptorGroup> descriptorGroups;
    InstanceContainer<DX12PipelineLayout> pipelineLayouts;
    InstanceContainer<DX12PipelineState> pipelineStates;
};
}
