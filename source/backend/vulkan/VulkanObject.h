#pragma once

#include <atomic>
#include "VulkanCommon.h"

namespace au::backend {

class VulkanObjectCounter {
public:
    using Object = uint32_t;

    static Object GetObjectCount();
    static Object CreateObject();
    static void DestroyObject();

private:
    static std::atomic<Object> counter;
    static std::atomic<Object> generator;
};

class VulkanBaseObject {
protected:
    explicit VulkanBaseObject();
    virtual ~VulkanBaseObject();

    VulkanObjectCounter::Object ObjectID() const;

private:
    VulkanObjectCounter::Object id = 0;
};

template <typename Object>
class VulkanObject : public VulkanBaseObject {
protected:
    explicit VulkanObject()
    {
        #if defined(DEBUG) || defined(_DEBUG)
        GP_LOG_D(TAG, "Construct Vulkan object `%s` instance `%d`: %p.",
            typeid(Object).name(), ObjectID(), this);
        #endif
    }

    ~VulkanObject() override
    {
        #if defined(DEBUG) || defined(_DEBUG)
        GP_LOG_D(TAG, "Deconstruct Vulkan object `%s` instance `%d`: %p.",
            typeid(Object).name(), ObjectID(), this);
        #endif
    }
};

}
