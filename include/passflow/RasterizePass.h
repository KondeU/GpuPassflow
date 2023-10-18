#pragma once

#include "BasePass.h"
#include "FrameResources.h"

namespace au::gp {

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

    void ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex);
    void ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex);

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

    std::vector<DynamicDescriptorManager> shaderResourceDescriptorHeaps;
    std::vector<DynamicDescriptorManager> imageSamplerDescriptorHeaps;
    std::vector<DynamicDescriptorManager> renderTargetDescriptorHeaps;
    std::vector<DynamicDescriptorManager> depthStencilDescriptorHeaps;

    struct RasterizePipelineCounters final {
        bool allowMultiObjects = false;

        unsigned int colorOutputCount = 0;
        unsigned int depthStencilOutputCount = 0;

        using CounterContainer = std::unordered_map<
            ShaderResourceProperties::ResourceSpace, unsigned int>;

        CounterContainer generalResourcesCounts; // shader resource
        CounterContainer imageSamplersCounts;    // image sampler

        CounterContainer reservedCounts = {
            { ShaderResourceProperties::ResourceSpace::PerObject, 1 },
            { ShaderResourceProperties::ResourceSpace::PerView,   1 },
            { ShaderResourceProperties::ResourceSpace::PerScene,  1 },
            { ShaderResourceProperties::ResourceSpace::PerPass,   1 }
        };

        unsigned int& ObjectShaderResourcesCount();
        unsigned int& ViewShaderResourcesCount();
        unsigned int& SceneShaderResourcesCount();
        unsigned int& PassShaderResourcesCount();

        unsigned int& ObjectImageSamplersCount();
        unsigned int& ViewImageSamplersCount();
        unsigned int& SceneImageSamplersCount();
        unsigned int& PassImageSamplersCount();

        unsigned int& ObjectsReservedCount();
        unsigned int& ViewsReservedCount();
        unsigned int& ScenesReservedCount();
        // There should be only one Pass normally, so do
        // not define the PassReservedCount() function.
    } rasterizePipelineCounters;

    std::vector<FrameResources> frameResources;
};

}
