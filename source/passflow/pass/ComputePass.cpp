#include "passflow/Passflow.h"

namespace au::gp {

ComputePass::ComputePass(Passflow& passflow) : BasePass(passflow)
{
    ClearFrameResources();
}

ComputePass::~ComputePass()
{
    CleanPipeline();
}

void ComputePass::MakeCurrent(const FRsKey& scene, const FRsKey& view)
{
    currentResources.frame = &AcquireStagingFrameResources();
    currentResources.scene = &(currentResources.frame->scenesResources[scene]);
    currentResources.view = &(currentResources.scene->viewsResources[view]);
}

bool ComputePass::AddDispatchItem(std::shared_ptr<DispatchItem> item)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddDrawItem, please MakeCurrent first!");
    }
    currentResources.scene->dispatchItems.emplace_back(item);
    return true;
}

bool ComputePass::AddPassResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(ConstantBuffer), please MakeCurrent first!");
    }
    currentResources.frame->passResources.constantBuffers[name] = buffer;
    return true;
}

bool ComputePass::AddPassResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(StructuredBuffer), please MakeCurrent first!");
    }
    currentResources.frame->passResources.structuredBuffers[name] = buffer;
    return true;
}

bool ComputePass::AddPassResource(const FRsKey& name, Resource<BaseTexture> texture)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(Texture), please MakeCurrent first!");
    }
    currentResources.frame->passResources.textures[name] = texture;
    return true;
}

bool ComputePass::AddPassResource(const FRsKey& name, Resource<Sampler> sampler)
{
    if (currentResources.frame == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddPassResource(Sampler), please MakeCurrent first!");
    }
    currentResources.frame->passResources.samplers[name] = sampler;
    return true;
}

bool ComputePass::AddSceneResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(ConstantBuffer), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.constantBuffers[name] = buffer;
    return true;
}

bool ComputePass::AddSceneResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(StructuredBuffer), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.structuredBuffers[name] = buffer;
    return true;
}

bool ComputePass::AddSceneResource(const FRsKey& name, Resource<BaseTexture> texture)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(Texture), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.textures[name] = texture;
    return true;
}

bool ComputePass::AddSceneResource(const FRsKey& name, Resource<Sampler> sampler)
{
    if (currentResources.scene == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddSceneResource(Sampler), please MakeCurrent first!");
    }
    currentResources.scene->sceneResources.samplers[name] = sampler;
    return true;
}

bool ComputePass::AddViewResource(const FRsKey& name, Resource<BaseConstantBuffer> buffer)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(ConstantBuffer), please MakeCurrent first!");
    }
    currentResources.view->viewResources.constantBuffers[name] = buffer;
    return true;
}

bool ComputePass::AddViewResource(const FRsKey& name, Resource<BaseStructuredBuffer> buffer)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(StructuredBuffer), please MakeCurrent first!");
    }
    currentResources.view->viewResources.structuredBuffers[name] = buffer;
    return true;
}

bool ComputePass::AddViewResource(const FRsKey& name, Resource<BaseTexture> texture)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(Texture), please MakeCurrent first!");
    }
    currentResources.view->viewResources.textures[name] = texture;
    return true;
}

bool ComputePass::AddViewResource(const FRsKey& name, Resource<Sampler> sampler)
{
    if (currentResources.view == nullptr) {
        GP_LOG_RETF_W(TAG, "Cannot AddViewResource(Sampler), please MakeCurrent first!");
    }
    currentResources.view->viewResources.samplers[name] = sampler;
    return true;
}

void ComputePass::ClearFrameResources()
{
    frameResources.clear(); // Clear all contents in order to release all shared pointer first.
    frameResources.resize(static_cast<size_t>(passflow.GetMultipleBufferingCount()) + 1);
    for (auto& each : frameResources) {
        each = std::make_shared<FrameResources>();
    }
    currentResources = {};
}

void ComputePass::InitializePipeline(rhi::Device* device)
{
    this->device = device;
    pipelineState = device->CreatePipelineState({ rhi::ShaderStage::Compute });
}

void ComputePass::DeclareProgram(const ProgramProperties& properties)
{
    for (const auto& [shaderStage, shaderProgram] : properties.shaders) {
        if (EnumCast(shaderStage) == EnumCast(rhi::ShaderStage::Compute)) {
            auto sourceType = rhi::Shader::Description::SourceType::SourceFile;
            if (shaderProgram.source.find('\n') != std::string::npos) {
                sourceType = rhi::Shader::Description::SourceType::Source;
            }
            computeShader = device->CreateShader({
                shaderStage, shaderProgram.source, shaderProgram.entry, sourceType });
            return;
        }
    }
}

void ComputePass::DeclareResource(const ShaderResourceProperties& properties)
{
    descriptorCounter.ClearResourcesAndSamplersCount();

    pipelineLayout = device->CreatePipelineLayout({});
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = EnumCast(resourceSpace);
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

bool ComputePass::BuildPipeline()
{
    if (!pipelineState) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, please initialize pipeline first.");
    }
    if (!pipelineLayout) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, please declare resource first.");
    }
    if (!computeShader) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, please declare program first.");
    }
    if (!computeShader->IsValid()) {
        GP_LOG_RETF_E(TAG, "Build pipeline failed, compute shader is invalid.");
    }

    pipelineState->SetPipelineLayout(pipelineLayout);
    pipelineState->SetShader(rhi::ShaderStage::Compute, computeShader);
    pipelineState->BuildState();

    shaderResourceDescriptorHeaps.resize(
        passflow.GetMultipleBufferingCount(),
        { device, rhi::DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(
        passflow.GetMultipleBufferingCount(),
        { device, rhi::DescriptorType::ImageSampler });
    return true;
}

void ComputePass::CleanPipeline()
{
    if (device) {
        if (computeShader) {
            device->DestroyShader(computeShader);
        }
        computeShader = nullptr;

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

        // Not owned the device, so set it to null simply.
        device = nullptr;

        // Reset recorded counters variable to default values.
        descriptorCounter.ClearAllCount();
    }
}

rhi::PipelineState* ComputePass::AcquirePipelineState()
{
    return pipelineState;
}

DynamicDescriptorManager&
ComputePass::AcquireShaderResourceDescriptorManager(unsigned int bufferingIndex)
{
    return shaderResourceDescriptorHeaps[bufferingIndex];
}

DynamicDescriptorManager&
ComputePass::AcquireImageSamplerDescriptorManager(unsigned int bufferingIndex)
{
    return imageSamplerDescriptorHeaps[bufferingIndex];
}

void ComputePass::UpdateFrameResources(unsigned int bufferingIndex)
{
    if (bufferingIndex >= (frameResources.size() - 1)) {
        GP_LOG_RET_E(TAG, "Cannot staging frame resources, target buffering index out of range!");
    }
    frameResources[bufferingIndex] = frameResources.back();
    frameResources.back() = std::make_shared<FrameResources>();
}

FrameResources& ComputePass::AcquireFrameResources(unsigned int bufferingIndex)
{
    if (bufferingIndex < (frameResources.size() - 1)) {
        return *(frameResources[bufferingIndex]);
    }
    GP_LOG_F(TAG, "Acquire frame resource out of range! "
                  "Return the staging frame resource instead.");
    return *(frameResources.back());
}

FrameResources& ComputePass::AcquireStagingFrameResources()
{
    return *(frameResources.back());
}

void ComputePass::ReserveEnoughDescriptors(unsigned int bufferingIndex)
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
}

}
