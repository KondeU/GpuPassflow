#include "passflow/Passflow.h"

namespace au::gp {

RasterizePass::RasterizePass(Passflow& passflow) : BasePass(passflow)
{
    ClearFrameResources();
}

RasterizePass::~RasterizePass()
{
    CleanPipeline();
}

void RasterizePass::MakeCurrent(const FRsKey& scene, const FRsKey& view)
{
    currentResources.frame = &AcquireStagingFrameResources();
    currentResources.scene = &(currentResources.frame->scenesResources[scene]);
    currentResources.view = &(currentResources.scene->viewsResources[view]);
}

bool RasterizePass::AddDrawItem(std::shared_ptr<DrawItem> item)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddDrawItem, please MakeCurrent first!");
    }
    currentResources.scene->drawItems.emplace_back(item);
    return true;
}

bool RasterizePass::AddPassResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(ConstantBuffer), please MakeCurrent first!");
    }
    currentResources.frame->passResources.constantBuffers[name] = buffer;
    return true;
}

bool RasterizePass::AddPassResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(StructuredBuffer), please MakeCurrent first!");
    }
    currentResources.frame->passResources.structuredBuffers[name] = buffer;
    return true;
}

bool RasterizePass::AddPassResource(const FRsKey& name, Resource<BaseTexture> texture)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(Texture), please MakeCurrent first!");
    }
    currentResources.frame->passResources.textures[name] = texture;
    return true;
}

bool RasterizePass::AddPassResource(const FRsKey& name, Resource<Sampler> sampler)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(Sampler), please MakeCurrent first!");
    }
    currentResources.frame->passResources.samplers[name] = sampler;
    return true;
}

bool RasterizePass::AddSceneResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(ConstantBuffer), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.constantBuffers[name] = buffer;
    return true;
}

bool RasterizePass::AddSceneResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(StructuredBuffer), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.structuredBuffers[name] = buffer;
    return true;
}

bool RasterizePass::AddSceneResource(const FRsKey& name, Resource<BaseTexture> texture)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(Texture), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.textures[name] = texture;
    return true;
}

bool RasterizePass::AddSceneResource(const FRsKey& name, Resource<Sampler> sampler)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(Sampler), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.samplers[name] = sampler;
    return true;
}

bool RasterizePass::AddViewResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(ConstantBuffer), please MakeCurrent first!");
    }
    currentResources.view->viewResources.constantBuffers[name] = buffer;
    return true;
}

bool RasterizePass::AddViewResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(StructuredBuffer), please MakeCurrent first!");
    }
    currentResources.view->viewResources.structuredBuffers[name] = buffer;
    return true;
}

bool RasterizePass::AddViewResource(const FRsKey& name, Resource<BaseTexture> texture)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(Texture), please MakeCurrent first!");
    }
    currentResources.view->viewResources.textures[name] = texture;
    return true;
}

bool RasterizePass::AddViewResource(const FRsKey& name, Resource<Sampler> sampler)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(Sampler), please MakeCurrent first!");
    }
    currentResources.view->viewResources.samplers[name] = sampler;
    return true;
}

bool RasterizePass::AddOutput(const FRsKey& name, Resource<ColorOutput> output)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddOutput(ColorOutput), please MakeCurrent first!");
    }
    currentResources.view->viewOutputs.colorOutputs[name] = output;
    return true;
}

bool RasterizePass::AddOutput(const FRsKey& name, Resource<DepthStencilOutput> output)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddOutput(DepthStencilOutput), please MakeCurrent first!");
    }
    currentResources.view->viewOutputs.depthStencilOutputs[name] = output;
    return true;
}

bool RasterizePass::AddOutput(const FRsKey& name, Resource<DisplayPresentOutput> output)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddOutput(DisplayPresentOutput), please MakeCurrent first!");
    }
    currentResources.view->viewOutputs.displayPresentOutputs[name] = output;
    return true;
}

void RasterizePass::ClearFrameResources()
{
    frameResources.clear(); // Clear all contents in order to release all shared pointer first.
    frameResources.resize(static_cast<size_t>(passflow.GetMultipleBufferingCount()) + 1);
    for (auto& each : frameResources) {
        each = std::make_shared<FrameResources>();
    }
    currentResources = {};
}

void RasterizePass::InitializePipeline(rhi::Device* device)
{
    this->device = device;
    pipelineState = device->CreatePipelineState({ rhi::ShaderStage::Graphics });
}

void RasterizePass::DeclareInput(const InputProperties& properties)
{
    inputVertexAttributes = device->CreateInputVertexAttributes();
    for (const auto& attribute : properties.vertexAttributes) {
        inputVertexAttributes->AddAttribute(attribute);
    }
    inputIndexAttribute = device->CreateInputIndexAttribute();
    inputIndexAttribute->SetAttribute(properties.indexAttribute);
}

void RasterizePass::DeclareOutput(const OutputProperties& properties)
{
    descriptorCounter.ClearOutputsCount();

    for (const auto& [outputSlot, outputAttribute] : properties.targets) {
        if (outputSlot == OutputProperties::OutputSlot::DS) {
            pipelineState->SetDepthStencilOutputFormat(outputAttribute.imagePixelFormat);
            descriptorCounter.depthStencilOutputsCount = 1;
        } else { // Color: C0,...,C7
            pipelineState->SetColorOutputFormat(
                EnumCast(outputSlot), outputAttribute.imagePixelFormat);
            descriptorCounter.colorOutputsCount++;
        }
    }
}

void RasterizePass::DeclareProgram(const ProgramProperties& properties)
{
    for (const auto& [shaderStage, shaderProgram] : properties.shaders) {
        if (EnumCast(shaderStage) & EnumCast(rhi::ShaderStage::Graphics)) {
            auto sourceType = rhi::Shader::Description::SourceType::SourceFile;
            if (shaderProgram.source.find('\n') != std::string::npos) {
                sourceType = rhi::Shader::Description::SourceType::Source;
            }
            programShaders[shaderStage] = device->CreateShader({
                shaderStage, shaderProgram.source, shaderProgram.entry, sourceType });
        }
    }
}

void RasterizePass::DeclareResource(const ShaderResourceProperties& properties)
{
    descriptorCounter.ClearResourcesAndSamplersCount();

    pipelineLayout = device->CreatePipelineLayout({});
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = EnumCast(resourceSpace); // Cast resource space to a integer.
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& attribute : resourceAttributes) {
            if (EnumCast(attribute.resourceType) &
                EnumCast(rhi::DescriptorType::ShaderResource)) {
                descriptorCounter.generalResourcesCount[space] += attribute.bindingPointCount;
            } else if (attribute.resourceType == rhi::DescriptorType::ImageSampler) {
                descriptorCounter.imageSamplersCount[space] += attribute.bindingPointCount;
            } else {
                GP_LOG_F(TAG, "Resource only can be the type of ShaderResource or ImageSampler.");
                // Note that continue will cause the descriptor group to be out of order,
                // but there is no way to solve it, so use LOG_F to print fatal error.
                continue;
            }

            if (attribute.bindingPointCount == 1) {
                group->AddDescriptor(
                    attribute.resourceType,
                    attribute.baseBindingPoint,
                    attribute.resourceVisibility);
            } else if (attribute.bindingPointCount > 1) {
                unsigned int begBP = attribute.baseBindingPoint;
                unsigned int endBP = attribute.baseBindingPoint +
                                     attribute.bindingPointCount - 1;
                group->AddDescriptors(
                    attribute.resourceType,
                    std::make_pair(begBP, endBP),
                    attribute.resourceVisibility);
            }
        }
        pipelineLayout->AddGroup(group);
    }
    pipelineLayout->BuildLayout();
}

bool RasterizePass::BuildPipeline()
{
    if (!pipelineState) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, please initialize pipeline first.");
    }
    if (!pipelineLayout) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, please declare resource first.");
    }
    if (programShaders.empty()) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, please declare program first.");
    }
    for (const auto& [stage, shader] : programShaders) {
        if (!shader->IsValid()) {
            GP_LOG_RETF_E(TAG, "Build pipeline failed, "
                "shader(stage is `%d`) is invalid.", EnumCast(stage));
        }
    }

    pipelineState->SetPipelineLayout(pipelineLayout);
    pipelineState->SetIndexAssembly(inputIndexAttribute);
    pipelineState->SetVertexAssembly(inputVertexAttributes);
    for (const auto& [stage, shader] : programShaders) {
        pipelineState->SetShader(stage, shader);
    }
    pipelineState->BuildState();

    shaderResourceDescriptorHeaps.resize(
        passflow.GetMultipleBufferingCount(),
        { device, rhi::DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(
        passflow.GetMultipleBufferingCount(),
        { device, rhi::DescriptorType::ImageSampler });
    renderTargetDescriptorHeaps.resize(
        passflow.GetMultipleBufferingCount(),
        { device, rhi::DescriptorType::ColorOutput });
    depthStencilDescriptorHeaps.resize(
        passflow.GetMultipleBufferingCount(),
        { device, rhi::DescriptorType::DepthStencil });
    return true;
}

void RasterizePass::CleanPipeline()
{
    if (device) {
        if (inputVertexAttributes) {
            device->DestroyInputVertexAttributes(inputVertexAttributes);
        }
        inputVertexAttributes = nullptr;
        if (inputIndexAttribute) {
            device->DestroyInputIndexAttribute(inputIndexAttribute);
        }
        inputIndexAttribute = nullptr;

        for (const auto& shader : programShaders) {
            device->DestroyShader(shader.second);
        }
        programShaders.clear();

        for (const auto& group : descriptorGroups) {
            device->DestroyDescriptorGroup(group.second);
        }
        descriptorGroups.clear();

        if (pipelineLayout) {
            device->DestroyPipelineLayout(pipelineLayout);
        }
        pipelineLayout = nullptr;
        if (pipelineState) {
            device->DestroyPipelineState(pipelineState);
        }
        pipelineState = nullptr;

        shaderResourceDescriptorHeaps.clear();
        imageSamplerDescriptorHeaps.clear();
        renderTargetDescriptorHeaps.clear();
        depthStencilDescriptorHeaps.clear();

        // Not owned the device, so set it to null simply.
        device = nullptr;

        // Reset recorded counters variable to default values.
        descriptorCounter.ClearAllCount();
    }
}

rhi::PipelineState* RasterizePass::AcquirePipelineState()
{
    return pipelineState;
}

rhi::InputIndexAttribute* RasterizePass::AcquireIndexAttribute()
{
    return inputIndexAttribute;
}

rhi::InputVertexAttributes* RasterizePass::AcquireVertexAttributes()
{
    return inputVertexAttributes;
}

DynamicDescriptorManager& RasterizePass::AcquireDescriptorManager(
    unsigned int bufferingIndex, rhi::DescriptorType descriptorType)
{
    if (EnumCast(descriptorType) &
        EnumCast(rhi::DescriptorType::ShaderResource)) {
        return shaderResourceDescriptorHeaps[bufferingIndex];
    }
    if (descriptorType == rhi::DescriptorType::ImageSampler) {
        return imageSamplerDescriptorHeaps[bufferingIndex];
    }
    if (descriptorType == rhi::DescriptorType::ColorOutput) {
        return renderTargetDescriptorHeaps[bufferingIndex];
    }
    if (descriptorType == rhi::DescriptorType::DepthStencil) {
        return depthStencilDescriptorHeaps[bufferingIndex];
    }
    // The program can not and should not run to here!
    GP_LOG_F(TAG, "No found suitable descriptor manager, internal logic error!");
    // Returning shaderResourceDescriptorHeaps only to avoid compile warning.
    return shaderResourceDescriptorHeaps[bufferingIndex];
}

void RasterizePass::UpdateFrameResources(unsigned int bufferingIndex)
{
    if (bufferingIndex >= (frameResources.size() - 1)) {
        GP_LOG_RET_E(TAG, "Cannot staging frame resources, target buffering index out of range!");
    }
    frameResources[bufferingIndex] = frameResources.back();
    frameResources.back() = std::make_shared<FrameResources>();
}

FrameResources& RasterizePass::AcquireFrameResources(unsigned int bufferingIndex)
{
    if (bufferingIndex < (frameResources.size() - 1)) {
        return *(frameResources[bufferingIndex]);
    }
    GP_LOG_F(TAG, "Acquire frame resource out of range! "
                  "Return the staging frame resource instead.");
    return *(frameResources.back());
}

FrameResources& RasterizePass::AcquireStagingFrameResources()
{
    return *(frameResources.back());
}

void RasterizePass::ReserveEnoughDescriptors(unsigned int bufferingIndex)
{
    const auto& scenesResources = AcquireStagingFrameResources().scenesResources;
    unsigned int sceneCount = scenesResources.size();
    unsigned int totalViewCount = 0, totalObjectCount = 0;
    for (const auto& [key, sceneResources] : scenesResources) {
        totalViewCount += sceneResources.viewsResources.size();
        totalObjectCount += sceneResources.drawItems.size();
    }

    shaderResourceDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounter.CalculateShaderResourcesCount(
            sceneCount, totalViewCount, totalObjectCount));
    imageSamplerDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounter.CalculateImageSamplersCount(
            sceneCount, totalViewCount, totalObjectCount));

    renderTargetDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounter.CalculateColorOutputsCount(totalViewCount));
    depthStencilDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounter.CalculateDepthStencilOutputsCount(totalViewCount));
}

}
