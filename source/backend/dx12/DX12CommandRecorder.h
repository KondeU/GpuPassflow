#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

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

    void RcBarrier(rhi::InputVertex* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::InputIndex* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::ResourceBuffer* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::ResourceBufferEx* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::ResourceImage* const resource,
        rhi::ResourceState before, rhi::ResourceState after) override;
    void RcBarrier(rhi::Swapchain* const swapchain,
        rhi::ResourceState before, rhi::ResourceState after) override;

    void RcUpload(const void* const data, size_t size,
        rhi::InputVertex* const destination, rhi::InputVertex* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::InputIndex* const destination, rhi::InputIndex* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::ResourceBuffer* const destination, rhi::ResourceBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::ResourceBufferEx* const destination, rhi::ResourceBufferEx* const staging) override;
    void RcUpload(const void* const data, size_t size,
        rhi::ResourceImage* const destination, rhi::ResourceImage* const staging) override;

    void RcCopy(rhi::InputVertex* const destination,
        rhi::InputVertex* const source) override;
    void RcCopy(rhi::InputIndex* const destination,
        rhi::InputIndex* const source) override;
    void RcCopy(rhi::ResourceBuffer* const destination,
        rhi::ResourceBuffer* const source) override;
    void RcCopy(rhi::ResourceBufferEx* const destination,
        rhi::ResourceBufferEx* const source) override;
    void RcCopy(rhi::ResourceImage* const destination,
        rhi::ResourceImage* const source) override;
    void RcCopy(rhi::Swapchain* const destination,
        rhi::ResourceImage* const source) override;

    void RcSetViewports(const std::vector<rhi::Viewport>& viewports) override;
    void RcSetScissors(const std::vector<rhi::Scissor>& scissors) override;

    void RcClearColorAttachment(rhi::Swapchain* const swapchain) override;
    void RcClearDepthStencilAttachment(rhi::Swapchain* const swapchain) override;
    void RcClearColorAttachment(rhi::Descriptor* const descriptor) override;
    void RcClearDepthStencilAttachment(rhi::Descriptor* const descriptor) override;
    void RcSetRenderAttachments(
        rhi::Swapchain* const swapchain,
        const std::vector<rhi::Descriptor*>& colorAttachments,
        const std::vector<rhi::Descriptor*>& depthStencilAttachments,
        bool descriptorsContinuous) override;

    void RcBeginPass(
        rhi::Swapchain* const swapchain,
        const std::vector<std::tuple<rhi::Descriptor*,
            rhi::PassAction, rhi::PassAction>>& colorOutputs,
        const std::vector<std::tuple<rhi::Descriptor*,
            rhi::PassAction, rhi::PassAction>>& depthStencil,
        bool writeBufferOrTextureResource) override;
    void RcEndPass() override;

    void RcSetPipeline(rhi::PipelineState* const pipelineState) override;

    void RcSetVertex(const std::vector<rhi::InputVertex*>& vertices,
        rhi::InputVertexAttributes* const attributes, unsigned int startSlot) override;
    void RcSetIndex(rhi::InputIndex* const index,
        rhi::InputIndexAttribute* const attribute) override;

    void RcSetDescriptorHeap(const std::vector<rhi::DescriptorHeap*>& heaps) override;

    void RcSetGraphicsDescriptor(
        unsigned int index, rhi::Descriptor* const descriptor) override;
    void RcSetGraphicsDescriptors(
        unsigned int index, const std::vector<rhi::Descriptor*>& descriptors) override;

    void RcSetComputeDescriptor(
        unsigned int index, rhi::Descriptor* const descriptor) override;
    void RcSetComputeDescriptors(
        unsigned int index, const std::vector<rhi::Descriptor*>& descriptors) override;

    void RcDraw(rhi::InputIndex* const index) override;

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
