#pragma once

#include "BasePass.h"
#include "resource/FrameResources.h"
#include "resource/DescriptorManager.h"

namespace au::gp {

class RasterizePass : public BasePass {
public:
    ~RasterizePass() override;

    // Set current Scene and View.
    virtual void MakeCurrent(const FRsKey& scene, const FRsKey& view);

    // [DrawCall] - Frame(Pass) -> Scene -> DrawItem
    virtual bool AddDrawItem(std::shared_ptr<DrawItem> item);

    // Frame(Pass) -> Resource
    virtual bool AddPassResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual bool AddPassResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual bool AddPassResource(const FRsKey& name, Resource<BaseTexture> texture);
    virtual bool AddPassResource(const FRsKey& name, Resource<Sampler> sampler);

    // Frame(Pass) -> Scene -> Resource
    virtual bool AddSceneResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual bool AddSceneResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual bool AddSceneResource(const FRsKey& name, Resource<BaseTexture> texture);
    virtual bool AddSceneResource(const FRsKey& name, Resource<Sampler> sampler);

    // Frame(Pass) -> Scene -> View -> Resource
    virtual bool AddViewResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer);
    virtual bool AddViewResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer);
    virtual bool AddViewResource(const FRsKey& name, Resource<BaseTexture> texture);
    virtual bool AddViewResource(const FRsKey& name, Resource<Sampler> sampler);

    // Frame(Pass) -> Scene -> View -> Output
    virtual bool AddOutput(const FRsKey& name, Resource<ColorOutput> output);
    virtual bool AddOutput(const FRsKey& name, Resource<DepthStencilOutput> output);
    virtual bool AddOutput(const FRsKey& name, Resource<DisplayPresentOutput> output);

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

    DynamicDescriptorManager& AcquireDescriptorManager(
        unsigned int bufferingIndex, rhi::DescriptorType descriptorType);

    void UpdateFrameResources(unsigned int bufferingIndex);
    FrameResources& AcquireFrameResources(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResources();

    void ReserveEnoughDescriptors(unsigned int bufferingIndex);

private:
    GP_LOG_TAG(RasterizePass);

    rhi::Device* device = nullptr; // Not owned!

    rhi::PipelineState* pipelineState = nullptr;
    rhi::PipelineLayout* pipelineLayout = nullptr;

    rhi::InputVertexAttributes* inputVertexAttributes = nullptr;
    rhi::InputIndexAttribute* inputIndexAttribute = nullptr;

    std::map<rhi::ShaderStage, rhi::Shader*> programShaders;
    std::map<uint8_t, rhi::DescriptorGroup*> descriptorGroups;

    DescriptorCounter descriptorCounter;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;
    std::vector<DynamicDescriptorManager> renderTargetDescriptorHeaps;
    std::vector<DynamicDescriptorManager> depthStencilDescriptorHeaps;

    std::vector<std::shared_ptr<FrameResources>> frameResources;
    struct CurrentSceneViewFrameResources final {
        FrameResources* frame = nullptr; // pass
        SceneResources* scene = nullptr;
        ViewResources* view = nullptr;
    } currentResources;
};

}
