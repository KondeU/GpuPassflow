#include "passflow/BasePass.h"

namespace au::gp {

BasePass::BasePass(Passflow& passflow) : passflow(passflow)
{
    multipleBufferingCount = Configuration::GetReference().GetMultipleBufferingCount();
}

rhi::InputVertexAttributes::Attribute
BasePass::InputProperties::MakeDefaultPositionVertexAttribute()
{
    return { rhi::VertexFormat::FLOAT32x3, "POSITION", 0, 0 };
}

BasePass::DynamicDescriptorManager::DynamicDescriptorManager(
    rhi::Device* device, rhi::DescriptorType type) : device(device)
{
    heapType = type;
}

BasePass::DynamicDescriptorManager::~DynamicDescriptorManager()
{
    FreeDescriptorHeap();
}

void BasePass::DynamicDescriptorManager::ReallocateDescriptorHeap(unsigned int descriptorCount)
{
    if (descriptorCount <= descriptors.size()) {
        return; // Only expand when the capacity is not enough, but never shrink.
    }
    FreeDescriptorHeap();
    descriptorHeap = device->CreateDescriptorHeap({ descriptorCount, heapType });
    descriptors.resize(descriptorCount, nullptr); // Vector of Descriptor pointers.
}

rhi::Descriptor* BasePass::DynamicDescriptorManager::AcquireDescriptor(unsigned int index)
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

rhi::DescriptorHeap* BasePass::DynamicDescriptorManager::AcquireDescriptorHeap()
{
    return descriptorHeap;
}

void BasePass::DynamicDescriptorManager::FreeDescriptorHeap()
{
    if (descriptorHeap) {
        // Destroy descriptor heap will destroy all descriptors in this heap.
        device->DestroyDescriptorHeap(descriptorHeap);
        descriptorHeap = nullptr;
        descriptors.clear();
    }
}

}
