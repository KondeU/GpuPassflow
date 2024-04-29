#pragma once

#include "BasePass.h"
#include "resource/FrameResources.h"

namespace au::gp {

class RasterizePass : public BasePass {
public:
    ~RasterizePass() override;

    // Set current Scene and View.
    virtual void MakeCurrent(const FRsKey& scene, const FRsKey& view);

    // Frame(Pass) -> Scene -> DrawItem
    virtual void AddDrawItem(std::shared_ptr<DrawItem> item);

    // Frame(Pass) -> Resource
    virtual void AddPassResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual void AddPassResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual void AddPassResource(const FRsKey& name, Resource<BaseTexture> buffer);
    virtual void AddPassResource(const FRsKey& name, Resource<Sampler> sampler);

    // Frame(Pass) -> Scene -> Resource
    virtual void AddSceneResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual void AddSceneResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual void AddSceneResource(const FRsKey& name, Resource<BaseTexture> buffer);
    virtual void AddSceneResource(const FRsKey& name, Resource<Sampler> sampler);

    // Frame(Pass) -> Scene -> View -> Resource
    virtual void AddViewResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual void AddViewResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual void AddViewResource(const FRsKey& name, Resource<BaseTexture> buffer);
    virtual void AddViewResource(const FRsKey& name, Resource<Sampler> sampler);

    // Frame(Pass) -> Scene -> View -> Output
    virtual void AddOutput(const FRsKey& name, Resource<ColorOutput> output);
    virtual void AddOutput(const FRsKey& name, Resource<DepthStencilOutput> output);
    virtual void AddOutput(const FRsKey& name, Resource<DisplayPresentOutput> output);

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

    FrameResources& AcquireFrameResources(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResources();

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

    std::vector<std::shared_ptr<FrameResources>> frameResources;
};

}
