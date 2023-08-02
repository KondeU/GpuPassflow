#pragma once

#include "DX12Common.h"

namespace au::backend {

class DX12ObjectCounter {
public:
    using Object = uint32_t;

    static Object GetObjectCount();
    static Object CreateObject();
    static void DestroyObject();

private:
    #ifdef GP_MULTI_THREADS_ACCESS_DXOBJECT
    using ObjectType = std::atomic<Object>;
    #else
    using ObjectType = Object;
    #endif
    static ObjectType counter;
    static ObjectType generator;
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
        AU_LOG_D(TAG, "Construct DX12 object `%s` instance `%d`: %p.",
            typeid(Object).name(), ObjectID(), this);
        #endif
    }

    ~DX12Object() override
    {
        #if defined(DEBUG) || defined(_DEBUG)
        AU_LOG_D(TAG, "Deconstruct DX12 object `%s` instance `%d`: %p.",
            typeid(Object).name(), ObjectID(), this);
        #endif
    }
};

}
