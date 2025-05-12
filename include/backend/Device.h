#pragma once

#include "VertexBuffer.h"
#include "VertexAttribute.h"
#include "IndexBuffer.h"
#include "IndexAttribute.h"
#include "ConstantBuffer.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"
#include "ImageBuffer.h"
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

    virtual VertexBuffer* CreateVertexBuffer(VertexBuffer::Description description) = 0;
    virtual bool DestroyVertexBuffer(VertexBuffer* instance) = 0;

    virtual VertexAttribute* CreateVertexAttribute() = 0;
    virtual bool DestroyVertexAttribute(VertexAttribute* instance) = 0;

    virtual IndexBuffer* CreateIndexBuffer(IndexBuffer::Description description) = 0;
    virtual bool DestroyIndexBuffer(IndexBuffer* instance) = 0;

    virtual IndexAttribute* CreateIndexAttribute() = 0;
    virtual bool DestroyIndexAttribute(IndexAttribute* instance) = 0;

    virtual UniformBuffer* CreateUniformBuffer(UniformBuffer::Description description) = 0;
    virtual bool DestroyUniformBuffer(UniformBuffer* instance) = 0;

    virtual StorageBuffer* CreateStorageBuffer(StorageBuffer::Description description) = 0;
    virtual bool DestroyResourceBuffer(StorageBuffer* instance) = 0;

    virtual ImageBuffer* CreateImageBuffer(ImageBuffer::Description description) = 0;
    virtual bool DestroyImageBuffer(ImageBuffer* instance) = 0;

    virtual ImageSampler* CreateImageSampler(ImageSampler::Description description) = 0;
    virtual bool DestroyImageSampler(ImageSampler* instance) = 0;

    virtual Swapchain* CreateSwapchain(Swapchain::Description description) = 0;
    virtual bool DestroySwapchain(Swapchain* instance) = 0;

    virtual Shader* CreateShader(Shader::Description description) = 0;
    virtual bool DestroyShader(Shader* instance) = 0;

    virtual PipelineLayout* CreatePipelineLayout(PipelineLayout::Description description) = 0;
    virtual bool DestroyPipelineLayout(PipelineLayout* instance) = 0;

    virtual PipelineState* CreatePipelineState(PipelineState::Description description) = 0;
    virtual bool DestroyPipelineState(PipelineState* instance) = 0;

    virtual DescriptorHeap* CreateDescriptorHeap(DescriptorHeap::Description description) = 0;
    virtual bool DestroyDescriptorHeap(DescriptorHeap* instance) = 0;

    virtual DescriptorGroup* CreateDescriptorGroup(DescriptorGroup::Description description) = 0;
    virtual bool DestroyDescriptorGroup(DescriptorGroup* instance) = 0;

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
