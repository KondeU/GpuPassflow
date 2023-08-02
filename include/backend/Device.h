#pragma once

#include "Shader.h"
#include "Swapchain.h"
#include "CommandRecorder.h"
#include "InputVertex.h"
#include "InputVertexAttributes.h"
#include "InputIndex.h"
#include "InputIndexAttribute.h"
#include "ResourceBuffer.h"
#include "ResourceImage.h"
#include "ImageSampler.h"
#include "DescriptorHeap.h"
#include "DescriptorGroup.h"
#include "PipelineLayout.h"
#include "PipelineState.h"

namespace au::rhi {

class Device {
public:
    struct Description {
        std::string device; // Selected adapter device name.
                            // Unsupported yet, use default adapter currently.
    };

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* instance) = 0;

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* instance) = 0;

    virtual CommandRecorder* CreateCommandRecorder(CommandRecorder::Description description) = 0;
    virtual bool DestroyCommandRecorder(CommandRecorder* instance) = 0;

    virtual InputVertex* CreateInputVertex(InputVertex::Description description) = 0;
    virtual bool DestroyInputVertex(InputVertex* instance) = 0;

    virtual InputVertexAttributes* CreateInputVertexAttributes() = 0;
    virtual bool DestroyInputVertexAttributes(InputVertexAttributes* instance) = 0;

    virtual InputIndex* CreateInputIndex(InputIndex::Description description) = 0;
    virtual bool DestroyInputIndex(InputIndex* instance) = 0;

    virtual InputIndexAttribute* CreateInputIndexAttribute() = 0;
    virtual bool DestroyInputIndexAttribute(InputIndexAttribute* instance) = 0;

    virtual ResourceBuffer* CreateResourceBuffer(ResourceBuffer::Description description) = 0;
    virtual bool DestroyResourceBuffer(ResourceBuffer* instance) = 0;

    virtual ResourceBufferEx* CreateResourceBuffer(ResourceBufferEx::Description description) = 0;
    virtual bool DestroyResourceBuffer(ResourceBufferEx* instance) = 0;

    virtual ResourceImage* CreateResourceImage(ResourceImage::Description description) = 0;
    virtual bool DestroyResourceImage(ResourceImage* instance) = 0;

    virtual ImageSampler* CreateImageSampler(ImageSampler::Description description) = 0;
    virtual bool DestroyImageSampler(ImageSampler* instance) = 0;

    virtual DescriptorHeap* CreateDescriptorHeap(DescriptorHeap::Description description) = 0;
    virtual bool DestroyDescriptorHeap(DescriptorHeap* instance) = 0;

    virtual DescriptorGroup* CreateDescriptorGroup(DescriptorGroup::Description description) = 0;
    virtual bool DestroyDescriptorGroup(DescriptorGroup* instance) = 0;

    virtual PipelineLayout* CreatePipelineLayout(PipelineLayout::Description description) = 0;
    virtual bool DestroyPipelineLayout(PipelineLayout* instance) = 0;

    virtual PipelineState* CreatePipelineState(PipelineState::Description description) = 0;
    virtual bool DestroyPipelineState(PipelineState* instance) = 0;

    virtual void WaitIdle() = 0;

    // This virtual function here is very ugly and should not have this interface.
    // However, I don't know where this implementation(reset the CommandAllocator)
    // should be placed is the most suitable, so finally I add this interface. :-(
    virtual void ReleaseCommandRecordersMemory(const std::string& commandContainer) = 0;

protected:
    Device() = default;
    virtual ~Device() = default;
};

}
