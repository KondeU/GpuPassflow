#include "DX12ConstantBuffer.h"

namespace au::backend {

DX12ConstantBuffer::DX12ConstantBuffer()
{
}

DX12ConstantBuffer::~DX12ConstantBuffer()
{
    Shutdown();
}

void DX12ConstantBuffer::Setup(Description description)
{
    this->description = description;
    buffer.resize(description.bufferBytesSize);
}

void DX12ConstantBuffer::Shutdown()
{
    description = { 0 };
    buffer.resize(0);
}

void* DX12ConstantBuffer::Map()
{
    return Buffer();
}

void DX12ConstantBuffer::Unmap()
{
}

uint32_t* DX12ConstantBuffer::Buffer()
{
    return buffer.data();
}

}
