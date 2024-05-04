#include "passflow/pass/resource/DescriptorManager.h"

namespace au::gp {

unsigned int DescriptorCounter::CalculateShaderResourcesCount(
    unsigned int sceneCount, unsigned int totalViewCount, unsigned int totalObjectCount) const
{
    return passShaderResourcesCount +
        sceneCount       * sceneShaderResourcesCount +
        totalViewCount   * viewShaderResourcesCount  +
        totalObjectCount * objectShaderResourcesCount;
}

unsigned int DescriptorCounter::CalculateImageSamplersCount(
    unsigned int sceneCount, unsigned int totalViewCount, unsigned int totalObjectCount) const
{
    return passImageSamplersCount +
        sceneCount       * sceneImageSamplersCount +
        totalViewCount   * viewImageSamplersCount  +
        totalObjectCount * objectImageSamplersCount;
}

unsigned int DescriptorCounter::CalculateColorOutputsCount(unsigned int totalViewCount) const
{
    return totalViewCount * colorOutputsCount;
}

unsigned int DescriptorCounter::CalculateDepthStencilOutputsCount(
                                                           unsigned int totalViewCount) const
{
    return totalViewCount * depthStencilOutputsCount;
}

void DescriptorCounter::ClearOutputsCount()
{
    colorOutputsCount = 0;
    depthStencilOutputsCount = 0;
}

void DescriptorCounter::ClearResourcesAndSamplersCount()
{
    generalResourcesCount.fill(0);
    imageSamplersCount.fill(0);
}

void DescriptorCounter::ClearAllCount()
{
    ClearOutputsCount();
    ClearResourcesAndSamplersCount();
}

DynamicDescriptorManager::DynamicDescriptorManager(
    rhi::Device* device, rhi::DescriptorType type) : device(device)
{
    heapType = type;
}

DynamicDescriptorManager::~DynamicDescriptorManager()
{
    FreeDescriptorHeap();
}

void DynamicDescriptorManager::ReallocateDescriptorHeap(unsigned int descriptorCount)
{
    if (descriptorCount <= descriptors.size()) {
        return; // Only expand when the capacity is not enough, but never shrink.
    }
    FreeDescriptorHeap();
    descriptorHeap = device->CreateDescriptorHeap({ descriptorCount, heapType });
    descriptors.resize(descriptorCount, nullptr); // Vector of Descriptor pointers.
}

rhi::Descriptor* DynamicDescriptorManager::AcquireDescriptor(unsigned int index)
{
    if (!descriptorHeap || index >= descriptors.size()) {
        GP_LOG_RETN_W(TAG, "Acquire descriptor out of range!");
    }
    auto& descriptor = descriptors[index];
    if (!descriptor) {
        // The Type used to create the DescriptorHeap only specifies the type of Heap, while
        // the Type used to create the Descriptor specifies the specific type. For example, the
        // type of DescriptorHeap is ShaderResource, and the type of Descriptor is ConstantBuffer.
        // And where, ShaderResource contains types such as ConstantBuffer and so on.
        // Here we directly use the Type of DescriptorHeap to allocate the Descriptor. There is
        // no problem in this way, but the abstract logic is not quite correct.
        descriptor = descriptorHeap->AllocateDescriptor({ heapType });
    }
    return descriptor;
}

rhi::DescriptorHeap* DynamicDescriptorManager::AcquireDescriptorHeap()
{
    return descriptorHeap;
}

void DynamicDescriptorManager::FreeDescriptorHeap()
{
    if (descriptorHeap) {
        // Destroy descriptor heap will destroy all descriptors in this heap.
        device->DestroyDescriptorHeap(descriptorHeap);
        descriptorHeap = nullptr;
        descriptors.clear();
    }
}

}
