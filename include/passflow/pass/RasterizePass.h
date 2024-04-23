#pragma once

#include "BasePass.h"
#include "resource/FrameResources.h"

namespace au::gp {

class RasterizePass : public BasePass {
public:
    ~RasterizePass() override;

    virtual void AddDrawItem(const FRsKey& scene, std::shared_ptr<DrawItem> item);

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

    virtual void AddOutput(const FRsKey& view,
        const FRsKey& name, Resource<ColorOutput> output);
    virtual void AddOutput(const FRsKey& view,
        const FRsKey& name, Resource<DepthStencilOutput> output);
    virtual void AddOutput(const FRsKey& view,
        const FRsKey& name, Resource<DisplayPresentOutput> output);

    void ClearFrameResources();

protected:
    explicit RasterizePass(Passflow& passflow);

    void InitializePipeline(rhi::Device* device);
    void DeclareInput(const InputProperties& properties);
    void DeclareOutput(const OutputProperties& properties);
    void DeclareProgram(const ProgramProperties& properties);
    void DeclareResource(const ShaderResourceProperties& properties);
    bool BuildPipeline();
    void CleanPipeline();

    // Use these functions to acquire pipeline state object or input attributes,
    // inherited classes can use them in the OnExecutePass function when drawing.
    rhi::PipelineState* AcquirePipelineState();
    rhi::InputIndexAttribute* AcquireIndexAttribute();
    rhi::InputVertexAttributes* AcquireVertexAttributes();

    void ReserveEnoughDescriptors(
        unsigned int bufferingIndex, unsigned int viewsCount, unsigned int scenesCount);

    DynamicDescriptorManager& AcquireDescriptorManager(
        unsigned int bufferingIndex, rhi::DescriptorType descriptorType);

    void UpdateDrawItems(unsigned int bufferingIndex);
    void UpdateFrameResources(unsigned int bufferingIndex);

    FrameResources& AcquireFrameResource(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResource();

private:
    GP_LOG_TAG(RasterizePass);

    rhi::Device* device = nullptr; // Not owned!

    rhi::PipelineState* pipelineState = nullptr;
    rhi::PipelineLayout* pipelineLayout = nullptr;

    rhi::InputVertexAttributes* inputVertexAttributes = nullptr;
    rhi::InputIndexAttribute* inputIndexAttribute = nullptr;

    std::map<rhi::ShaderStage, rhi::Shader*> programShaders;
    std::map<uint8_t, rhi::DescriptorGroup*> descriptorGroups;

    DescriptorCounters descriptorCounters;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;
    std::vector<DynamicDescriptorManager> renderTargetDescriptorHeaps;
    std::vector<DynamicDescriptorManager> depthStencilDescriptorHeaps;

    std::vector<FrameResources> frameResources;
};

}
