#pragma once

#include <atomic>
#include "DX12Common.h"

namespace au::backend {

class DX12ObjectCounter {
public:
    using Object = uint32_t;

    static Object GetObjectCount();
    static Object CreateObject();
    static void DestroyObject();

private:
    static std::atomic<Object> counter;
    static std::atomic<Object> generator;
};

class DX12BaseObject {
protected:
    explicit DX12BaseObject();
    virtual ~DX12BaseObject();

    DX12ObjectCounter::Object ObjectID() const;

private:
    DX12ObjectCounter::Object id = 0;
};

template <typename Object>
class DX12Object : public DX12BaseObject {
protected:
    explicit DX12Object()
    {
        #if defined(DEBUG) || defined(_DEBUG)
        GP_LOG_D(TAG, "Construct DX12 object `%s` instance `%d`: %p.",
            typeid(Object).name(), ObjectID(), this);
        #endif
    }

    ~DX12Object() override
    {
        #if defined(DEBUG) || defined(_DEBUG)
        GP_LOG_D(TAG, "Deconstruct DX12 object `%s` instance `%d`: %p.",
            typeid(Object).name(), ObjectID(), this);
        #endif
    }

    //static_assert(std::is_base_of<DX12Object<Object>, Object>::value,
    //    "Input template type Object must derived from DX12Object!");
};

}
