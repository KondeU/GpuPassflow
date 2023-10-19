#include "passflow/ComputePass.h"
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

unsigned int ComputePass::AddDispatchItem(std::weak_ptr<DispatchItem> item)
{
    if (auto lockedItem = item.lock()) {
        AcquireStagingFrameResource().dispatchItems.emplace_back(lockedItem);
        return static_cast<unsigned int>(AcquireStagingFrameResource().dispatchItems.size() - 1);
    }
    GP_LOG_W(TAG, "Dispatch item has been released and cannot be added to pass for computing.");
    return std::numeric_limits<unsigned int>::max();
}

bool ComputePass::VerifyDispatchItemIndex(unsigned int index)
{
    return (index < AcquireStagingFrameResource().dispatchItems.size()) ? true : false;
}

void ComputePass::ImportFrameResource(
    const std::string& name, Resource<BaseConstantBuffer> buffer)
{
    AcquireStagingFrameResource().frameResources.constantBuffers[name] = buffer;
}

void ComputePass::ImportFrameResource(
    const std::string& name, Resource<BaseStructuredBuffer> buffer)
{
    AcquireStagingFrameResource().frameResources.structuredBuffers[name] = buffer;
}

void ComputePass::ImportFrameResource(
    const std::string& name, Resource<BaseTexture> texture)
{
    AcquireStagingFrameResource().frameResources.textures[name] = texture;
}

void ComputePass::ImportFrameResource(
    const std::string& name, Resource<Sampler> sampler)
{
    AcquireStagingFrameResource().frameResources.samplers[name] = sampler;
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
    descriptorCounters.generalResourcesCounts.clear();
    descriptorCounters.imageSamplersCounts.clear();

    pipelineLayout = device->CreatePipelineLayout({});
    for (const auto& [resourceSpace, resourceAttributes] : properties.resources) {
        auto space = EnumCast(resourceSpace);
        auto group = descriptorGroups[space] = device->CreateDescriptorGroup({ space });
        for (const auto& attribute : resourceAttributes) {
            if (resourceSpace == ShaderResourceProperties::ResourceSpace::PerObject) {
                if (EnumCast(attribute.resourceType) &
                    EnumCast(rhi::DescriptorType::ShaderResource)) {
                    descriptorCounters.objectShaderResourcesCount +=
                        attribute.bindingPointCount;
                } else {
                    GP_LOG_F(TAG, "PerObject only can have the resource type of ShaderResource.");
                    // Note that continue will cause the descriptor group to be out of order,
                    // but there is no way to solve it, so use LOG_F to print fatal error.
                    continue;
                }
            } else {
                if (EnumCast(attribute.resourceType) &
                    EnumCast(rhi::DescriptorType::ShaderResource)) {
                    descriptorCounters.shaderResourcesCount += attribute.bindingPointCount;
                } else if (attribute.resourceType == rhi::DescriptorType::ImageSampler) {
                    descriptorCounters.imageSamplersCount += attribute.bindingPointCount;
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

    unsigned int mbc = multipleBufferingCount;
    shaderResourceDescriptorHeaps.resize(mbc, { device, rhi::DescriptorType::ShaderResource });
    imageSamplerDescriptorHeaps.resize(mbc, { device, rhi::DescriptorType::ImageSampler });
    for (unsigned int index = 0; index < mbc; index++) {
        ReserveEnoughAllTypesDescriptors(index);
    }

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
        descriptorCounters = DescriptorCounters();
    }
}

rhi::PipelineState* ComputePass::AcquirePipelineState()
{
    return pipelineState;
}

void ComputePass::ReserveEnoughShaderResourceDescriptors(unsigned int bufferingIndex)
{
    do {
        if (descriptorCounters.reservedObjectsCount >=
            AcquireStagingFrameResource().dispatchItems.size()) {
            break;
        }
    } while (descriptorCounters.reservedObjectsCount <<= 1);

    if (descriptorCounters.reservedObjectsCount == 0) {
        GP_LOG_F(TAG, "Dispatch items count overflow and no enough descriptors!");
    }

    shaderResourceDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounters.CalculateShaderResourcesCount());
}

void ComputePass::ReserveEnoughAllTypesDescriptors(unsigned int bufferingIndex)
{
    ReserveEnoughShaderResourceDescriptors(bufferingIndex);

    imageSamplerDescriptorHeaps[bufferingIndex].ReallocateDescriptorHeap(
        descriptorCounters.CalculateImageSamplersCount());
}

BasePass::DynamicDescriptorManager&
ComputePass::AcquireShaderResourceDescriptorManager(unsigned int bufferingIndex)
{
    return shaderResourceDescriptorHeaps[bufferingIndex];
}

BasePass::DynamicDescriptorManager&
ComputePass::AcquireImageSamplerDescriptorManager(unsigned int bufferingIndex)
{
    return imageSamplerDescriptorHeaps[bufferingIndex];
}

void ComputePass::UpdateDispatchItems(unsigned int bufferingIndex)
{
    FrameResources& updating = AcquireFrameResource(bufferingIndex);
    FrameResources& staging = AcquireStagingFrameResource();

    auto Updater = [](auto& updating, auto& staging) {
        updating.clear();
        updating.swap(staging);
    };

    Updater(updating.dispatchItems, staging.dispatchItems);
}

void ComputePass::UpdateFrameResources(unsigned int bufferingIndex)
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
}

FrameResources& ComputePass::AcquireFrameResource(unsigned int bufferingIndex)
{
    if (bufferingIndex < (frameResources.size() - 1)) {
        return frameResources[bufferingIndex];
    }
    GP_LOG_F(TAG, "Acquire frame resource out of range! "
                  "Return the staging frame resource instead.");
    return frameResources.back();
}

FrameResources& ComputePass::AcquireStagingFrameResource()
{
    return frameResources.back();
}

void ComputePass::ClearFrameResources()
{
    frameResources.clear(); // Clear all contents in order to release all shared pointer first.
    frameResources.resize(static_cast<size_t>(multipleBufferingCount) + 1);
}

}
