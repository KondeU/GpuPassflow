#include "DX12BaseObject.h"

namespace au::backend {

std::atomic<DX12ObjectCounter::Object> DX12ObjectCounter::counter{};
std::atomic<DX12ObjectCounter::Object> DX12ObjectCounter::generator{};

DX12ObjectCounter::Object DX12ObjectCounter::GetObjectCount()
{
    return counter;
}

DX12ObjectCounter::Object DX12ObjectCounter::CreateObject()
{
    counter++;
    return (generator++);
}

void DX12ObjectCounter::DestroyObject()
{
    counter--;
}

DX12BaseObject::DX12BaseObject()
{
    id = DX12ObjectCounter::CreateObject();
}

DX12BaseObject::~DX12BaseObject()
{
    DX12ObjectCounter::DestroyObject();
}

DX12ObjectCounter::Object DX12BaseObject::ObjectID() const
{
    return id;
}

}
