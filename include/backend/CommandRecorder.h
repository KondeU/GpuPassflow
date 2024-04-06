#pragma once

#include <vector>
#include "BasicTypes.h"

namespace au::rhi {

class InputVertex;
class InputIndex;
class InputVertexAttributes;
class InputIndexAttribute;
class ResourceConstantBuffer;
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

    virtual void RcBarrier(InputVertex* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(InputIndex* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceConstantBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceStorageBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceImage* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(Swapchain* const swapchain,
        ResourceState before, ResourceState after) = 0;

    virtual void RcUpload(const void* const data, size_t size,
        InputVertex* const destination, InputVertex* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        InputIndex* const destination, InputIndex* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceConstantBuffer* const destination, ResourceConstantBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceStorageBuffer* const destination, ResourceStorageBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceImage* const destination, ResourceImage* const staging) = 0;

    virtual void RcCopy(InputVertex* const dst, InputVertex* const src) = 0;
    virtual void RcCopy(InputIndex* const dst, InputIndex* const src) = 0;
    virtual void RcCopy(ResourceConstantBuffer* const dst, ResourceConstantBuffer* const src) = 0;
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

    virtual void RcSetVertex(const std::vector<InputVertex*>& vertices,
        InputVertexAttributes* const attributes, unsigned int startSlot = 0) = 0;
    virtual void RcSetIndex(InputIndex* const index, InputIndexAttribute* const attribute) = 0;

    virtual void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) = 0;

    virtual void RcSetGraphicsDescriptor(
        unsigned int index, Descriptor* const descriptor) = 0;
    virtual void RcSetGraphicsDescriptors(
        unsigned int index, const std::vector<Descriptor*>& descriptors) = 0;

    virtual void RcSetComputeDescriptor(
        unsigned int index, Descriptor* const descriptor) = 0;
    virtual void RcSetComputeDescriptors(
        unsigned int index, const std::vector<Descriptor*>& descriptors) = 0;

    virtual void RcDraw(InputIndex* const index) = 0;

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
