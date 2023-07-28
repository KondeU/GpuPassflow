#pragma once

#include "DX12Common.h"

namespace au::backend {
class DX12ObjectCounter : public common::GlobalSingleton<DX12ObjectCounter> {
public:
    using Object = uint32_t;

    Object GetObjectCount() const
    {
        return counter;
    }

    Object Create()
    {
        counter++;
        return (generator++);
    }

    void Destroy()
    {
        counter--;
    }

private:
    #ifdef AU_OPT_MULTI_THREADS
    using ObjectType = std::atomic<Object>;
    #else
    using ObjectType = Object;
    #endif

    ObjectType counter = 0;
    ObjectType generator = 0;
};

class DX12BaseObject {
protected:
    DX12BaseObject()
    {
        id = DX12ObjectCounter::GetReference().Create();
    }

    virtual ~DX12BaseObject()
    {
        DX12ObjectCounter::GetReference().Destroy();
    }

    DX12ObjectCounter::Object ObjectID() const
    {
        return id;
    }

private:
    DX12ObjectCounter::Object id = 0;
};

template <typename Object>
class DX12Object : public DX12BaseObject {
protected:
    DX12Object()
    {
        #if defined(DEBUG) || defined(_DEBUG)
        AU_LOG_D(TAG, "Construct DX12 object `%s` instance `%d`: %p.",
            ObjectUniqueName, ObjectID(), this);
        #endif
    }

    ~DX12Object() override
    {
        #if defined(DEBUG) || defined(_DEBUG)
        AU_LOG_D(TAG, "Deconstruct DX12 object `%s` instance `%d`: %p.",
            ObjectUniqueName, ObjectID(), this);
        #endif
    }

private:
    const char* ObjectUniqueName = typeid(Object).name();
};
}
