#pragma once

#include "InputVertex.h"
#include "InputVertexAttributes.h"
#include "InputIndex.h"
#include "InputIndexAttribute.h"
#include "ResourceConstantBuffer.h"
#include "ResourceStorageBuffer.h"
#include "ResourceImage.h"
#include "ImageSampler.h"
#include "Swapchain.h"
#include "Shader.h"
#include "PipelineLayout.h"
#include "PipelineState.h"
#include "DescriptorHeap.h"
#include "DescriptorGroup.h"
#include "CommandRecorder.h"

namespace au::rhi {

class Device {
public:
    struct Description {
        std::string adaptor;
    };

    //----------------------------------------//
    //             Input Assembly             //
    //----------------------------------------//

    virtual InputVertex* CreateInputVertex(InputVertex::Description description) = 0;
    virtual bool DestroyInputVertex(InputVertex* instance) = 0;

    virtual InputVertexAttributes* CreateInputVertexAttributes() = 0;
    virtual bool DestroyInputVertexAttributes(InputVertexAttributes* instance) = 0;

    virtual InputIndex* CreateInputIndex(InputIndex::Description description) = 0;
    virtual bool DestroyInputIndex(InputIndex* instance) = 0;

    virtual InputIndexAttribute* CreateInputIndexAttribute() = 0;
    virtual bool DestroyInputIndexAttribute(InputIndexAttribute* instance) = 0;

    //----------------------------------------//
    //                Resource                //
    //----------------------------------------//

    virtual ResourceConstantBuffer* CreateResourceBuffer(
        ResourceConstantBuffer::Description description) = 0;
    virtual bool DestroyResourceBuffer(ResourceConstantBuffer* instance) = 0;

    virtual ResourceStorageBuffer* CreateResourceBuffer(
        ResourceStorageBuffer::Description description) = 0;
    virtual bool DestroyResourceBuffer(ResourceStorageBuffer* instance) = 0;

    virtual ResourceImage* CreateResourceImage(ResourceImage::Description description) = 0;
    virtual bool DestroyResourceImage(ResourceImage* instance) = 0;

    virtual ImageSampler* CreateImageSampler(ImageSampler::Description description) = 0;
    virtual bool DestroyImageSampler(ImageSampler* instance) = 0;

    //----------------------------------------//
    //               Swapchain                //
    //----------------------------------------//

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* instance) = 0;

    //----------------------------------------//
    //                Pipeline                //
    //----------------------------------------//

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* instance) = 0;

    virtual PipelineLayout* CreatePipelineLayout(PipelineLayout::Description description) = 0;
    virtual bool DestroyPipelineLayout(PipelineLayout* instance) = 0;

    virtual PipelineState* CreatePipelineState(PipelineState::Description description) = 0;
    virtual bool DestroyPipelineState(PipelineState* instance) = 0;

    //----------------------------------------//
    //               Descriptor               //
    //----------------------------------------//

    virtual DescriptorHeap* CreateDescriptorHeap(DescriptorHeap::Description description) = 0;
    virtual bool DestroyDescriptorHeap(DescriptorHeap* instance) = 0;

    virtual DescriptorGroup* CreateDescriptorGroup(DescriptorGroup::Description description) = 0;
    virtual bool DestroyDescriptorGroup(DescriptorGroup* instance) = 0;

    //----------------------------------------//
    //                Command                 //
    //----------------------------------------//

    virtual CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) = 0;
    virtual bool DestroyCommandRecorder(CommandRecorder* instance) = 0;

    // This virtual function here is very ugly and should not have this interface.
    // However, I don't know where this implementation(reset the CommandAllocator)
    // should be placed is the most suitable, so finally I add this interface. :-(
    virtual void ReleaseCommandRecordersMemory(const std::string& commandContainer) = 0;

    // Force the CPU to synchronize with the GPU.
    virtual void WaitIdle() = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};

}
