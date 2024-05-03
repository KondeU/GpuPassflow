#pragma once

#include <array>
#include "PassProperties.h"

namespace au::gp {

struct DescriptorCounter final {
    unsigned int colorOutputsCount = 0;        // Associated with View.
    unsigned int depthStencilOutputsCount = 0; // Associated with View.

    using CounterContainer = std::array<
        unsigned int, ShaderResourceProperties::ResourceSpaceCount>;
    CounterContainer generalResourcesCount; // shader resource
    CounterContainer imageSamplersCount;    // image sampler

    unsigned int& objectShaderResourcesCount = generalResourcesCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerObject)];
    unsigned int& viewShaderResourcesCount = generalResourcesCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerView)];
    unsigned int& sceneShaderResourcesCount = generalResourcesCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerScene)];
    unsigned int& passShaderResourcesCount = generalResourcesCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerPass)];

    unsigned int& objectImageSamplersCount = imageSamplersCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerObject)];
    unsigned int& viewImageSamplersCount = imageSamplersCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerView)];
    unsigned int& sceneImageSamplersCount = imageSamplersCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerScene)];
    unsigned int& passImageSamplersCount = imageSamplersCount[
        EnumCast(ShaderResourceProperties::ResourceSpace::PerPass)];

    unsigned int CalculateShaderResourcesCount(unsigned int sceneCount,
        unsigned int totalViewCount, unsigned int totalObjectCount) const;
    unsigned int CalculateImageSamplersCount(unsigned int sceneCount,
        unsigned int totalViewCount, unsigned int totalObjectCount) const;
    unsigned int CalculateColorOutputsCount(unsigned int totalViewCount) const;
    unsigned int CalculateDepthStencilOutputsCount(unsigned int totalViewCount) const;

    void ClearOutputsCount();
    void ClearResourcesAndSamplersCount();
    void ClearAllCount();
};

class DynamicDescriptorManager final {
public:
    // No explicit to allow calling vector.resize(N, {...})
    DynamicDescriptorManager(rhi::Device* device, rhi::DescriptorType type);
    ~DynamicDescriptorManager();

    void ReallocateDescriptorHeap(unsigned int descriptorCount);
    rhi::Descriptor* AcquireDescriptor(unsigned int index);
    rhi::DescriptorHeap* AcquireDescriptorHeap();

private:
    GP_LOG_TAG(DynamicDescriptorManager);

    void FreeDescriptorHeap();

    rhi::DescriptorType heapType;
    rhi::Device* device; // Not owned!
    rhi::DescriptorHeap* descriptorHeap = nullptr;
    std::vector<rhi::Descriptor*> descriptors;
};

}
