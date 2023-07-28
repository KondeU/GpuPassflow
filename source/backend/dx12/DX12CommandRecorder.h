#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;

class DX12CommandRecorder : public CommandRecorder
    , DX12Object<DX12CommandRecorder> {
public:
    explicit DX12CommandRecorder(DX12Device& device);
    ~DX12CommandRecorder() override;

    void Setup(Description description);
    void Shutdown();

    void BeginRecord() override;
    void EndRecord() override;

    void RcBarrier(InputVertex* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(InputIndex* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceBuffer* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceBufferEx* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceImage* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(Swapchain* const swapchain,
        ResourceState before, ResourceState after) override;

    void RcUpload(const void* const data, size_t size,
        InputVertex* const destination, InputVertex* const staging) override;
    void RcUpload(const void* const data, size_t size,
        InputIndex* const destination, InputIndex* const staging) override;
    void RcUpload(const void* const data, size_t size,
        ResourceBuffer* const destination, ResourceBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        ResourceBufferEx* const destination, ResourceBufferEx* const staging) override;
    void RcUpload(const void* const data, size_t size,
        ResourceImage* const destination, ResourceImage* const staging) override;

    void RcCopy(InputVertex* const destination, InputVertex* const source) override;
    void RcCopy(InputIndex* const destination, InputIndex* const source) override;
    void RcCopy(ResourceBuffer* const destination, ResourceBuffer* const source) override;
    void RcCopy(ResourceBufferEx* const destination, ResourceBufferEx* const source) override;
    void RcCopy(ResourceImage* const destination, ResourceImage* const source) override;
    void RcCopy(Swapchain* const destination, ResourceImage* const source) override;

    void RcSetViewports(const std::vector<Viewport>& viewports) override;
    void RcSetScissors(const std::vector<Scissor>& scissors) override;

    void RcClearColorAttachment(Swapchain* const swapchain) override;
    void RcClearDepthStencilAttachment(Swapchain* const swapchain) override;
    void RcClearColorAttachment(Descriptor* const descriptor) override;
    void RcClearDepthStencilAttachment(Descriptor* const descriptor) override;
    void RcSetRenderAttachments(
        Swapchain* const swapchain,
        const std::vector<Descriptor*>& colorAttachments,
        const std::vector<Descriptor*>& depthStencilAttachments,
        bool descriptorsContinuous) override;

    void RcBeginPass(
        Swapchain* const swapchain,
        const std::vector<std::tuple<Descriptor*, PassAction, PassAction>>& colorOutputs,
        const std::vector<std::tuple<Descriptor*, PassAction, PassAction>>& depthStencil,
        bool writeBufferOrTextureResource) override;
    void RcEndPass() override;

    void RcSetPipeline(PipelineState* const pipelineState) override;

    void RcSetVertex(const std::vector<InputVertex*>& vertices,
        InputVertexAttributes* const attributes, unsigned int startSlot) override;
    void RcSetIndex(InputIndex* const index, InputIndexAttribute* const attribute) override;

    void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) override;

    void RcSetGraphicsDescriptor(
        unsigned int index, Descriptor* const descriptor) override;
    void RcSetGraphicsDescriptors(
        unsigned int index, const std::vector<Descriptor*>& descriptors) override;

    void RcSetComputeDescriptor(
        unsigned int index, Descriptor* const descriptor) override;
    void RcSetComputeDescriptors(
        unsigned int index, const std::vector<Descriptor*>& descriptors) override;

    void RcDraw(InputIndex* const index) override;

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
