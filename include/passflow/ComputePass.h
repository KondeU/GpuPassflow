#pragma once

#include "BasePass.h"
#include "FrameResources.h"

namespace au::gp {

class ComputePass : public BasePass {
public:
    ~ComputePass() override;

    virtual unsigned int AddDispatchItem(std::weak_ptr<DispatchItem> item);
    virtual bool VerifyDispatchItemIndex(unsigned int index);

    virtual void ImportFrameResource(
        const std::string& name, Resource<BaseConstantBuffer> buffer);
    virtual void ImportFrameResource(
        const std::string& name, Resource<BaseStructuredBuffer> buffer);
    virtual void ImportFrameResource(
        const std::string& name, Resource<BaseTexture> texture);
    virtual void ImportFrameResource(
        const std::string& name, Resource<Sampler> sampler);

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

    void ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex);
    void ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex);

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

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;

    struct ComputePipelineCounters final {
        unsigned int reservedObjectsCount = 1;
        unsigned int objectShaderResourcesCount = 0;
        unsigned int shaderResourcesCount = 0;
        unsigned int imageSamplersCount = 0;
    } computePipelineCounters;

    std::vector<FrameResources> frameResources;
};

}
