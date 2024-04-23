#pragma once

#include "BasePass.h"
#include "resource/FrameResources.h"

namespace au::gp {

class ComputePass : public BasePass {
public:
    ~ComputePass() override;

    virtual void AddDispatchItem(const FRsKey& scene, std::shared_ptr<DispatchItem> item);

    virtual void AddPassResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual void AddPassResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual void AddPassResource(const FRsKey& name, Resource<BaseTexture> buffer);
    virtual void AddPassResource(const FRsKey& name, Resource<Sampler> sampler);

    virtual void AddSceneResource(const FRsKey& scene,
        const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual void AddSceneResource(const FRsKey& scene,
        const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual void AddSceneResource(const FRsKey& scene,
        const FRsKey& name, Resource<BaseTexture> buffer);
    virtual void AddSceneResource(const FRsKey& scene,
        const FRsKey& name, Resource<Sampler> sampler);

    virtual void AddViewResource(const FRsKey& view,
        const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual void AddViewResource(const FRsKey& view,
        const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual void AddViewResource(const FRsKey& view,
        const FRsKey& name, Resource<BaseTexture> buffer);
    virtual void AddViewResource(const FRsKey& view,
        const FRsKey& name, Resource<Sampler> sampler);

    void ClearFrameResources();

protected:
    explicit ComputePass(Passflow& passflow);

    void InitializePipeline(rhi::Device* device);
    void DeclareProgram(const ProgramProperties& properties);
    void DeclareResource(const ShaderResourceProperties& properties);
    bool BuildPipeline();
    void CleanPipeline();

    // Use this function to acquire pipeline state object, inherited
    // classes can use it in the OnExecutePass function when dispatching.
    rhi::PipelineState* AcquirePipelineState();

    void ReserveEnoughDescriptors(
        unsigned int bufferingIndex, unsigned int viewsCount, unsigned int scenesCount);

    DynamicDescriptorManager& AcquireShaderResourceDescriptorManager(unsigned int bufferingIndex);
    DynamicDescriptorManager& AcquireImageSamplerDescriptorManager(unsigned int bufferingIndex);

    void UpdateDispatchItems(unsigned int bufferingIndex);
    void UpdateFrameResources(unsigned int bufferingIndex);

    FrameResources& AcquireFrameResource(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResource();

private:
    GP_LOG_TAG(ComputePass);

    rhi::Device* device = nullptr; // Not owned!

    rhi::PipelineState* pipelineState = nullptr;
    rhi::PipelineLayout* pipelineLayout = nullptr;

    rhi::Shader* computeShader = nullptr;

    std::map<uint8_t, rhi::DescriptorGroup*> descriptorGroups;

    DescriptorCounters descriptorCounters;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;

    std::vector<FrameResources> frameResources;
};

}
