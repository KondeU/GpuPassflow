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

void RasterizePass::AddDrawItem(const FRsKey& scene, std::shared_ptr<DrawItem> item)
{
    AcquireStagingFrameResource().scenesResources.drawItems[scene].emplace_back(item);
}

unsigned int RasterizePass::AddDrawItem(std::weak_ptr<DrawItem> item)
{
    if (auto lockedItem = item.lock()) {
        AcquireStagingFrameResource().drawItems.emplace_back(lockedItem);
        return static_cast<unsigned int>(AcquireStagingFrameResource().drawItems.size() - 1);
    }
    GP_LOG_W(TAG, "Draw item has been released and cannot be added to pass for rendering.");
    return std::numeric_limits<unsigned int>::max();
}

bool RasterizePass::VerifyDrawItemIndex(unsigned int index)
{
    return (index < AcquireStagingFrameResource().drawItems.size()) ? true : false;
}

void RasterizePass::ImportFrameResource(
    const std::string& name, Resource<BaseConstantBuffer> buffer)
{
    AcquireStagingFrameResource().frameResources.constantBuffers[name] = buffer;
}

void RasterizePass::ImportFrameResource(
    const std::string& name, Resource<BaseStructuredBuffer> buffer)
{
    AcquireStagingFrameResource().frameResources.structuredBuffers[name] = buffer;
}

void RasterizePass::ImportFrameResource(
    const std::string& name, Resource<BaseTexture> texture)
{
    AcquireStagingFrameResource().frameResources.textures[name] = texture;
}

void RasterizePass::ImportFrameResource(
    const std::string& name, Resource<Sampler> sampler)
{
    AcquireStagingFrameResource().frameResources.samplers[name] = sampler;
}

void RasterizePass::ImportFrameOutput(
    const std::string& name, Resource<ColorOutput> output)
{
    AcquireStagingFrameResource().frameOutputs.colorOutputs[name] = output;
}

void RasterizePass::ImportFrameOutput(
    const std::string& name, Resource<DepthStencilOutput> output)
{
    AcquireStagingFrameResource().frameOutputs.depthStencilOutputs[name] = output;
}

void RasterizePass::ImportFrameOutput(
    const std::string& name, Resource<DisplayPresentOutput> output)
{
    AcquireStagingFrameResource().frameOutputs.displayPresentOutputs[name] = output;
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

    descriptorCounters.allowMultiObjects = properties.multipleObjects;
    if (descriptorCounters.allowMultiObjects) {
        // The number of objects reserved initially.
        // If the number of objects added by calling AddDrawItem exceeds it, it will
        // be doubled when the reserved descriptors in descriptor heap is not enough.
        descriptorCounters.ObjectsReservedCount() = (1 << 4); // 16
    }
}

void RasterizePass::DeclareOutput(const OutputProperties& properties)
{
    descriptorCounters.colorOutputCount = 0;
    descriptorCounters.depthStencilOutputCount = 0;

    for (const auto& [outputSlot, outputAttribute] : properties.targets) {
        if (outputSlot == OutputProperties::OutputSlot::DS) {
            pipelineState->SetDepthStencilOutputFormat(outputAttribute.imagePixelFormat);
            descriptorCounters.depthStencilOutputCount = 1;
        } else { // Color: C0,...,C7
            pipelineState->SetColorOutputFormat(
                EnumCast(outputSlot), outputAttribute.imagePixelFormat);
            descriptorCounters.colorOutputCount++;
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
    descriptorCounters.generalResourcesCounts.clear();
    descriptorCounters.imageSamplersCounts.clear();

    pipelineLayout = device->CreatePipelineLayout({});
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = EnumCast(resourceSpace);
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& attribute : resourceAttributes) {
            if (EnumCast(attribute.resourceType) &
                EnumCast(rhi::DescriptorType::ShaderResource)) {
                descriptorCounters.generalResourcesCounts
                    [resourceSpace] += attribute.bindingPointCount;
            } else if (attribute.resourceType == rhi::DescriptorType::ImageSampler) {
                descriptorCounters.imageSamplersCounts
                    [resourceSpace] += attribute.bindingPointCount;
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

    shaderResourceDescriptorHeaps.resize(multipleBufferingCount,
        { device, rhi::DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(multipleBufferingCount,
        { device, rhi::DescriptorType::ImageSampler });
    renderTargetDescriptorHeaps.resize(multipleBufferingCount,
        { device, rhi::DescriptorType::ColorOutput });
    depthStencilDescriptorHeaps.resize(multipleBufferingCount,
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
        descriptorCounters = DescriptorCounters();
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

void RasterizePass::ReserveEnoughDescriptors(
    unsigned int bufferingIndex, unsigned int viewsCount, unsigned int scenesCount)
{
    do {
        if (descriptorCounters.ObjectsReservedCount() >=
            AcquireStagingFrameResource().drawItems.size()) {
            break;
        }
    } while (descriptorCounters.ObjectsReservedCount() <<= 1);

    if (descriptorCounters.ObjectsReservedCount() == 0) {
        GP_LOG_F(TAG, "Draw items count overflow and no enough descriptors!");
    }

    descriptorCounters.ViewsReservedCount() = viewsCount;
    descriptorCounters.ScenesReservedCount() = scenesCount;

    shaderResourceDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounters.CalculateShaderResourcesCount());

    imageSamplerDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounters.CalculateImageSamplersCount());

    renderTargetDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounters.colorOutputCount);

    depthStencilDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounters.depthStencilOutputCount);
}

BasePass::DynamicDescriptorManager& RasterizePass::AcquireDescriptorManager(
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

void RasterizePass::UpdateDrawItems(unsigned int bufferingIndex)
{
    FrameResources& updating = AcquireFrameResource(bufferingIndex);
    FrameResources& staging = AcquireStagingFrameResource();

    auto Updater = [](auto& updating, auto& staging) {
        updating.clear();
        updating.swap(staging);
    };

    Updater(updating.drawItems, staging.drawItems);
}

void RasterizePass::UpdateFrameResources(unsigned int bufferingIndex)
{
    FrameResources& updating = AcquireFrameResource(bufferingIndex);
    FrameResources& staging = AcquireStagingFrameResource();

    auto Updater = [](auto& updating, auto& staging) {
        for (const auto& [name, resource] : staging) {
            updating[name] = resource;
        }
        staging.clear();
    };

    Updater(updating.frameResources.textures,
             staging.frameResources.textures);
    Updater(updating.frameResources.constantBuffers,
             staging.frameResources.constantBuffers);
    Updater(updating.frameResources.structuredBuffers,
             staging.frameResources.structuredBuffers);
    Updater(updating.frameResources.samplers,
             staging.frameResources.samplers);

    Updater(updating.frameOutputs.colorOutputs,
             staging.frameOutputs.colorOutputs);
    Updater(updating.frameOutputs.depthStencilOutputs,
             staging.frameOutputs.depthStencilOutputs);
    Updater(updating.frameOutputs.displayPresentOutputs,
             staging.frameOutputs.displayPresentOutputs);
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

void RasterizePass::ClearFrameResources()
{
    frameResources.clear(); // Clear all contents in order to release all shared pointer first.
    frameResources.resize(static_cast<size_t>(multipleBufferingCount) + 1);
    for (auto& each : frameResources) {
        each = std::make_shared<FrameResources>();
    }
}

}
