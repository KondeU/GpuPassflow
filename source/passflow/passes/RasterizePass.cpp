#include "passflow/RasterizePass.h"
#include "passflow/Passflow.h"

namespace au::passflow {

RasterizePass::RasterizePass(Passflow& passflow) : BasePass(passflow)
{
    ClearFrameResources();
}

RasterizePass::~RasterizePass()
{
    CleanPipeline();
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

void RasterizePass::InitializePipeline(backend::Device* device)
{
    this->device = device;
    pipelineState = device->CreatePipelineState({ ShaderStage::Graphics });
}

void RasterizePass::DeclareInput(const InputProperties& properties)
{
    inputVertexAttributes = device->CreateInputVertexAttributes();
    for (const auto& attribute : properties.vertexAttributes) {
        inputVertexAttributes->AddAttribute(attribute);
    }
    inputIndexAttribute = device->CreateInputIndexAttribute();
    inputIndexAttribute->SetAttribute(properties.indexAttribute);

    rasterizePipelineCounters.allowMultiObjects = properties.multipleObjects;
    if (rasterizePipelineCounters.allowMultiObjects) {
        // The number of objects reserved initially.
        // If the number of objects added by calling AddDrawItem exceeds it, it will
        // be doubled when the reserved descriptors in descriptor heap is not enough.
        rasterizePipelineCounters.reservedObjectsCount = 16;
    }
}

void RasterizePass::DeclareOutput(const OutputProperties& properties)
{
    rasterizePipelineCounters.colorOutputCount = 0;
    rasterizePipelineCounters.depthStencilOutputCount = 0;

    for (const auto& [outputSlot, outputAttribute] : properties.targets) {
        if (outputSlot == OutputProperties::OutputSlot::DS) {
            pipelineState->SetDepthStencilOutputFormat(outputAttribute.imagePixelFormat);
            rasterizePipelineCounters.depthStencilOutputCount = 1;
        } else { // Color: C0,...,C7
            pipelineState->SetColorOutputFormat(
                framework::EnumCast(outputSlot), outputAttribute.imagePixelFormat);
            rasterizePipelineCounters.colorOutputCount++;
        }
    }
}

void RasterizePass::DeclareProgram(const ProgramProperties& properties)
{
    for (const auto& [shaderStage, shaderProgram] : properties.shaders) {
        if (framework::EnumCast(shaderStage) & framework::EnumCast(ShaderStage::Graphics)) {
            auto sourceType = backend::Shader::Description::SourceType::SourceFile;
            if (shaderProgram.source.find('\n') != std::string::npos) {
                sourceType = backend::Shader::Description::SourceType::Source;
            }
            programShaders[shaderStage] = device->CreateShader({
                shaderStage, shaderProgram.source, shaderProgram.entry, sourceType });
        }
    }
}

void RasterizePass::DeclareResource(const ShaderResourceProperties& properties)
{
    rasterizePipelineCounters.objectShaderResourcesCount = 0;
    rasterizePipelineCounters.shaderResourcesCount = 0;
    rasterizePipelineCounters.imageSamplersCount = 0;

    pipelineLayout = device->CreatePipelineLayout({});
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = framework::EnumCast(resourceSpace);
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& attribute : resourceAttributes) {
            if (resourceSpace == ShaderResourceProperties::ResourceSpace::PerObject) {
                if (framework::EnumCast(attribute.resourceType) &
                    framework::EnumCast(DescriptorType::ShaderResource)) {
                    rasterizePipelineCounters.objectShaderResourcesCount +=
                        attribute.bindingPointCount;
                } else {
                    GP_LOG_F(TAG, "PerObject only can have the resource type of ShaderResource.");
                    // Note that continue will cause the descriptor group to be out of order,
                    // but there is no way to solve it, so use LOG_F to print fatal error.
                    continue;
                }
            } else {
                if (framework::EnumCast(attribute.resourceType) &
                    framework::EnumCast(DescriptorType::ShaderResource)) {
                    rasterizePipelineCounters.shaderResourcesCount += attribute.bindingPointCount;
                } else if (attribute.resourceType == DescriptorType::ImageSampler) {
                    rasterizePipelineCounters.imageSamplersCount += attribute.bindingPointCount;
                } else {
                    GP_LOG_F(TAG, "Resource only can be the type of ShaderResource/ImageSampler.");
                    continue; // NB: Ditto.
                }
            }

            if (attribute.bindingPointCount == 1) {
                group->AddDescriptor(
                    attribute.resourceType,
                    attribute.baseBindingPoint,
                    attribute.resourceVisibility);
            } else if (attribute.bindingPointCount > 1) {
                unsigned int begBp = attribute.baseBindingPoint;
                unsigned int endBp = attribute.baseBindingPoint +
                                     attribute.bindingPointCount - 1;
                group->AddDescriptors(
                    attribute.resourceType,
                    std::make_pair(begBp, endBp),
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
                "shader(stage is `%d`) is invalid.", framework::EnumCast(stage));
        }
    }

    pipelineState->SetPipelineLayout(pipelineLayout);
    pipelineState->SetIndexAssembly(inputIndexAttribute);
    pipelineState->SetVertexAssembly(inputVertexAttributes);
    for (const auto& [stage, shader] : programShaders) {
        pipelineState->SetShader(stage, shader);
    }
    pipelineState->BuildState();

    unsigned int mbc = multipleBufferingCount;
    shaderResourceDescriptorHeaps.resize(mbc, { device, DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(mbc, { device, DescriptorType::ImageSampler });
    renderTargetDescriptorHeaps.resize(mbc, { device, DescriptorType::ColorOutput });
    depthStencilDescriptorHeaps.resize(mbc, { device, DescriptorType::DepthStencil });
    for (unsigned int index = 0; index < mbc; index++) {
        ReserveEnoughAllTypesDescriptors(index);
    }

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
        rasterizePipelineCounters = RasterizePipelineCounters();
    }
}

backend::PipelineState* RasterizePass::AcquirePipelineState()
{
    return pipelineState;
}

backend::InputIndexAttribute* RasterizePass::AcquireIndexAttribute()
{
    return inputIndexAttribute;
}

backend::InputVertexAttributes* RasterizePass::AcquireVertexAttributes()
{
    return inputVertexAttributes;
}

void RasterizePass::ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex)
{
    do {
        if (rasterizePipelineCounters.reservedObjectsCount >=
            AcquireStagingFrameResource().drawItems.size()) {
            break;
        }
    } while (rasterizePipelineCounters.reservedObjectsCount <<= 1);

    if (rasterizePipelineCounters.reservedObjectsCount == 0) {
        GP_LOG_F(TAG, "Draw items count overflow and no enough descriptors!");
    }

    shaderResourceDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.shaderResourcesCount +
        rasterizePipelineCounters.reservedObjectsCount *
        rasterizePipelineCounters.objectShaderResourcesCount);
}

void RasterizePass::ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex)
{
    ReserveEnoughShaderResourceDescriptors(bufferingIndex);

    imageSamplerDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.imageSamplersCount);

    renderTargetDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.colorOutputCount);

    depthStencilDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        rasterizePipelineCounters.depthStencilOutputCount);
}

BasePass::DynamicDescriptorManager& RasterizePass::AcquireDescriptorManager(
    unsigned int bufferingIndex, DescriptorType descriptorType)
{
    const std::unordered_map<DescriptorType, std::vector<DynamicDescriptorManager>*> map = {
        { DescriptorType::ShaderResource, &shaderResourceDescriptorHeaps },
        { DescriptorType::ImageSampler,   &imageSamplerDescriptorHeaps   },
        { DescriptorType::ColorOutput,    &renderTargetDescriptorHeaps   },
        { DescriptorType::DepthStencil,   &depthStencilDescriptorHeaps   }
    };
    if (framework::EnumCast(descriptorType) &
        framework::EnumCast(DescriptorType::ShaderResource)) {
        descriptorType = DescriptorType::ShaderResource;
    }
    return map.at(descriptorType)->at(bufferingIndex);
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

FrameResources& RasterizePass::AcquireFrameResource(unsigned int bufferingIndex)
{
    if (bufferingIndex < (frameResources.size() - 1)) {
        return frameResources[bufferingIndex];
    }
    GP_LOG_F(TAG, "Acquire frame resource out of range! "
                  "Return the staging frame resource instead.");
    return frameResources.back();
}

FrameResources& RasterizePass::AcquireStagingFrameResource()
{
    return frameResources.back();
}

void RasterizePass::ClearFrameResources()
{
    frameResources.clear(); // Clear all contents in order to release all shared pointer first.
    frameResources.resize(static_cast<size_t>(multipleBufferingCount) + 1);
}

}
