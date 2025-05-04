#pragma once

#include <vector>
#include "BasicTypes.h"

namespace au::rhi {

class VertexBuffer;
class IndexBuffer;
class VertexAttribute;
class IndexAttribute;
class UniformBuffer;
class ResourceStorageBuffer;
class ResourceImage;
class Descriptor;
class DescriptorHeap;
class PipelineState;

class CommandRecorder {
public:
    struct Description {
        std::string container;
        CommandType commandType;

        Description(
            const std::string& commandContainer,
            CommandType commandType = CommandType::Graphics)
            : container(commandContainer)
            , commandType(commandType)
        {}
    };

    virtual void BeginRecord() = 0;
    virtual void EndRecord() = 0;

    virtual void RcBarrier(VertexBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(IndexBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(UniformBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceStorageBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceImage* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(Swapchain* const swapchain,
        ResourceState before, ResourceState after) = 0;

    virtual void RcUpload(const void* const data, size_t size,
        VertexBuffer* const destination, VertexBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        IndexBuffer* const destination, IndexBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        UniformBuffer* const destination, UniformBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceStorageBuffer* const destination, ResourceStorageBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceImage* const destination, ResourceImage* const staging) = 0;

    virtual void RcCopy(VertexBuffer* const dst, VertexBuffer* const src) = 0;
    virtual void RcCopy(IndexBuffer* const dst, IndexBuffer* const src) = 0;
    virtual void RcCopy(UniformBuffer* const dst, UniformBuffer* const src) = 0;
    virtual void RcCopy(ResourceStorageBuffer* const dst, ResourceStorageBuffer* const src) = 0;
    virtual void RcCopy(ResourceImage* const dst, ResourceImage* const src) = 0;
    virtual void RcCopy(Swapchain* const dst, ResourceImage* const src) = 0;

    virtual void RcSetViewports(const std::vector<Viewport>& viewports) = 0;
    virtual void RcSetScissors(const std::vector<Scissor>& scissors) = 0;

    //----------------------------------------//
    //           Global Driven Mode           //
    //----------------------------------------//

    virtual void RcClearColorAttachment(Swapchain* const swapchain) = 0;
    virtual void RcClearDepthStencilAttachment(Swapchain* const swapchain) = 0;
    virtual void RcClearColorAttachment(Descriptor* const descriptor) = 0;
    virtual void RcClearDepthStencilAttachment(Descriptor* const descriptor) = 0;
    virtual void RcSetRenderAttachments(
        Swapchain* const swapchain,
        const std::vector<Descriptor*>& colorAttachments,
        const std::vector<Descriptor*>& depthStencilAttachments,
        bool descriptorsContinuous = false) = 0;

    //----------------------------------------//
    //            Pass Driven Mode            //
    //----------------------------------------//

    virtual void RcBeginPass(
        Swapchain* const swapchain,
        const std::vector<std::tuple<Descriptor*, PassAction, PassAction>>& colorOutputs,
        const std::vector<std::tuple<Descriptor*, PassAction, PassAction>>& depthStencil,
        bool writeBufferOrTextureResource = false) = 0;
    virtual void RcEndPass() = 0;

    //----------------------------------------//

    virtual void RcSetPipeline(PipelineState* const pipelineState) = 0;

    virtual void RcSetVertex(const std::vector<VertexBuffer*>& vertices,
        VertexAttribute* const attributes, unsigned int startSlot = 0) = 0;
    virtual void RcSetIndex(IndexBuffer* const index, IndexAttribute* const attribute) = 0;

    virtual void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) = 0;

    virtual void RcSetGraphicsDescriptor(
        unsigned int index, Descriptor* const descriptor) = 0;
    virtual void RcSetGraphicsDescriptors(
        unsigned int index, const std::vector<Descriptor*>& descriptors) = 0;

    virtual void RcSetComputeDescriptor(
        unsigned int index, Descriptor* const descriptor) = 0;
    virtual void RcSetComputeDescriptors(
        unsigned int index, const std::vector<Descriptor*>& descriptors) = 0;

    virtual void RcDraw(IndexBuffer* const index) = 0;

    virtual void RcDispatch(
        unsigned int xThreadGroupsCount,
        unsigned int yThreadGroupsCount,
        unsigned int zThreadGroupsCount) = 0;

    virtual void Submit() = 0;
    virtual void Wait() = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};

}
