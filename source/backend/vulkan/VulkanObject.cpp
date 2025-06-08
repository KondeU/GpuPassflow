#include "VulkanObject.h"

namespace au::backend {

std::atomic<VulkanObjectCounter::Object> VulkanObjectCounter::counter{0};
std::atomic<VulkanObjectCounter::Object> VulkanObjectCounter::generator{0};

VulkanObjectCounter::Object VulkanObjectCounter::GetObjectCount()
{
    return counter.load();
}

VulkanObjectCounter::Object VulkanObjectCounter::CreateObject()
{
    counter++;
    return ++generator;
}

void VulkanObjectCounter::DestroyObject()
{
    counter--;
}

VulkanBaseObject::VulkanBaseObject()
{
    id = VulkanObjectCounter::CreateObject();
}

VulkanBaseObject::~VulkanBaseObject()
{
    VulkanObjectCounter::DestroyObject();
}

VulkanObjectCounter::Object VulkanBaseObject::ObjectID() const
{
    return id;
}

} 