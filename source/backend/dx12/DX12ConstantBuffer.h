#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12ConstantBuffer : public rhi::ConstantBuffer
    , DX12Object<DX12ConstantBuffer> {
public:
    explicit DX12ConstantBuffer();
    ~DX12ConstantBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    uint32_t* Buffer();

private:
    Description description{ 0 };
    std::vector<uint32_t> buffer;
};

}
