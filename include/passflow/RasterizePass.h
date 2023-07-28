#pragma once

#include "BasePass.h"
#include "FrameResources.h"

namespace au::passflow {

class RasterizePass : public BasePass {
public:
    ~RasterizePass() override;

    virtual unsigned int AddDrawItem(std::weak_ptr<DrawItem> item);
    virtual bool VerifyDrawItemIndex(unsigned int index);

    virtual void ImportFrameResource(
        const std::string& name, Resource<BaseConstantBuffer> buffer);
    virtual void ImportFrameResource(
        const std::string& name, Resource<BaseStructuredBuffer> buffer);
    virtual void ImportFrameResource(
        const std::string& name, Resource<BaseTexture> texture);
    virtual void ImportFrameResource(
        const std::string& name, Resource<Sampler> sampler);

    virtual void ImportFrameOutput(
        const std::string& name, Resource<ColorOutput> output);
    virtual void ImportFrameOutput(
        const std::string& name, Resource<DepthStencilOutput> output);
    virtual void ImportFrameOutput(
        const std::string& name, Resource<DisplayPresentOutput> output);

    void ClearFrameResources();

protected:
    explicit RasterizePass(Passflow& passflow);

    void InitializePipeline(backend::Device* device);
    void DeclareInput(const InputProperties& properties);
    void DeclareOutput(const OutputProperties& properties);
    void DeclareProgram(const ProgramProperties& properties);
    void DeclareResource(const ShaderResourceProperties& properties);
    bool BuildPipeline();
    void CleanPipeline();

    // Use these functions to acquire pipeline state object or input attributes,
    // inherited classes can use them in the OnExecutePass function when drawing.
    backend::PipelineState* AcquirePipelineState();
    backend::InputIndexAttribute* AcquireIndexAttribute();
    backend::InputVertexAttributes* AcquireVertexAttributes();

    void ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex);
    void ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex);

    DynamicDescriptorManager& AcquireDescriptorManager(
        unsigned int bufferingIndex, DescriptorType descriptorType);

    void UpdateDrawItems(unsigned int bufferingIndex);
    void UpdateFrameResources(unsigned int bufferingIndex);

    FrameResources& AcquireFrameResource(unsigned int bufferingIndex);
    FrameResources& AcquireStagingFrameResource();

private:
    AU_LOG_TAG(RasterizePass);

    backend::Device* device = nullptr; // Not owned!

    backend::PipelineState* pipelineState = nullptr;
    backend::PipelineLayout* pipelineLayout = nullptr;

    backend::InputVertexAttributes* inputVertexAttributes = nullptr;
    backend::InputIndexAttribute* inputIndexAttribute = nullptr;

    std::map<ShaderStage, backend::Shader*> programShaders;
    std::map<uint8_t, backend::DescriptorGroup*> descriptorGroups;

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;
    std::vector<DynamicDescriptorManager> renderTargetDescriptorHeaps;
    std::vector<DynamicDescriptorManager> depthStencilDescriptorHeaps;

    struct RasterizePipelineCounters final {
        bool allowMultiObjects = false;
        unsigned int colorOutputCount = 0;
        unsigned int depthStencilOutputCount = 0;
        unsigned int reservedObjectsCount = 1;
        unsigned int objectShaderResourcesCount = 0;
        unsigned int shaderResourcesCount = 0;
        unsigned int imageSamplersCount = 0;
    } rasterizePipelineCounters;

    std::vector<FrameResources> frameResources;
};

}
