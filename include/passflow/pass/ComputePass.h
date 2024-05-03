#pragma once

#include "BasePass.h"
#include "resource/FrameResources.h"

namespace au::gp {

class ComputePass : public BasePass {
public:
    ~ComputePass() override;

    // Set current Scene and View.
    virtual void MakeCurrent(const FRsKey& scene, const FRsKey& view);

    // [Dispatch] - Frame(Pass) -> Scene -> DispatchItem
    virtual bool AddDispatchItem(std::shared_ptr<DispatchItem> item);

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

    DynamicDescriptorManager& AcquireShaderResourceDescriptorManager(unsigned int bufferingIndex);
    DynamicDescriptorManager& AcquireImageSamplerDescriptorManager(unsigned int bufferingIndex);

    void UpdateFrameResources(unsigned int bufferingIndex);
    FrameResources& AcquireFrameResources(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResources();

    void ReserveEnoughDescriptors(unsigned int bufferingIndex);

private:
    GP_LOG_TAG(ComputePass);

    rhi::Device* device = nullptr; // Not owned!

    rhi::PipelineState* pipelineState = nullptr;
    rhi::PipelineLayout* pipelineLayout = nullptr;

    rhi::Shader* computeShader = nullptr;

    std::map<uint8_t, rhi::DescriptorGroup*> descriptorGroups;

    DescriptorCounter descriptorCounter;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;

    std::vector<std::shared_ptr<FrameResources>> frameResources;
    struct CurrentSceneViewFrameResources final {
        FrameResources* frame = nullptr; // pass
        SceneResources* scene = nullptr;
        ViewResources* view = nullptr;
    } currentResources;
};

}
