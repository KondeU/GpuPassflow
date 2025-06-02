#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12Device;

class DX12CommandRecorder : public rhi::CommandRecorder
    , DX12Object<DX12CommandRecorder> {
public:
    explicit DX12CommandRecorder(DX12Device& device);
    ~DX12CommandRecorder() override;

    void Setup(Description description);
    void Shutdown();

    void BeginRecord() override;
    void EndRecord() override;

    void RcBarrier(rhi::VertexBuffer* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::IndexBuffer* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::UniformBuffer* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::StorageBuffer* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::ImageBuffer* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::Swapchain* const swapchain,
        rhi::ResourceState before, rhi::ResourceState after) override;

    void RcUpload(const void* const data, size_t size,
        rhi::VertexBuffer* const destination, rhi::VertexBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::IndexBuffer* const destination, rhi::IndexBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::UniformBuffer* const destination,
        rhi::UniformBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::StorageBuffer* const destination,
        rhi::StorageBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::ImageBuffer* const destination, rhi::ImageBuffer* const staging) override;

    void RcCopy(rhi::VertexBuffer* const destination, rhi::VertexBuffer* const source) override;
    void RcCopy(rhi::IndexBuffer* const destination, rhi::IndexBuffer* const source) override;
    void RcCopy(rhi::UniformBuffer* const destination, rhi::UniformBuffer* const source) override;
    void RcCopy(rhi::StorageBuffer* const destination, rhi::StorageBuffer* const source) override;
    void RcCopy(rhi::ImageBuffer* const destination, rhi::ImageBuffer* const source) override;
    void RcCopy(rhi::Swapchain* const destination, rhi::ImageBuffer* const source) override;

    void RcSetViewports(const std::vector<rhi::Viewport>& viewports) override;
    void RcSetScissors(const std::vector<rhi::Scissor>& scissors) override;

    void RcBeginPass(
        rhi::Swapchain* const swapchain,
        const std::vector<std::tuple<rhi::Descriptor*,
            rhi::PassAction, rhi::PassAction>>& colorOutputs,
        const std::vector<std::tuple<rhi::Descriptor*,
            rhi::PassAction, rhi::PassAction>>& depthStencil,
        bool writeBufferOrTextureResource) override;
    void RcEndPass() override;

    void RcSetPipeline(rhi::PipelineState* const pipelineState) override;

    void RcSetVertex(const std::vector<rhi::VertexBuffer*>& vertices,
        rhi::VertexAttribute* const attributes, unsigned int startSlot) override;
    void RcSetIndex(rhi::IndexBuffer* const index,
        rhi::IndexAttribute* const attribute) override;

    void RcSetDescriptorHeap(const std::vector<rhi::DescriptorHeap*>& heaps) override;

    void RcSetGraphicsDescriptor(
        unsigned int index, rhi::Descriptor* const descriptor) override;
    void RcSetGraphicsDescriptors(
        unsigned int index, const std::vector<rhi::Descriptor*>& descriptors) override;

    void RcSetComputeDescriptor(
        unsigned int index, rhi::Descriptor* const descriptor) override;
    void RcSetComputeDescriptors(
        unsigned int index, const std::vector<rhi::Descriptor*>& descriptors) override;

    void RcDraw(rhi::IndexBuffer* const index) override;

    void RcDispatch(
        unsigned int xThreadGroupsCount,
        unsigned int yThreadGroupsCount,
        unsigned int zThreadGroupsCount) override;

    void Submit() override;
    void Wait() override;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ "" };
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> recorder; // CommandList4 for RenderPass

    UINT64 currentFence = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
};

}
