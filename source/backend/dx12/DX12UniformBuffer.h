#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Object.h"

namespace au::backend {

class DX12Device;

class DX12UniformBuffer : public rhi::UniformBuffer
    , DX12Object<DX12UniformBuffer> {
public:
    explicit DX12UniformBuffer(DX12Device& device);
    ~DX12UniformBuffer() override;

    bool Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    unsigned int GetBufferBytesSize() const;
    unsigned int GetAllocatedBytesSize() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

protected:
    static unsigned int CalculateAlignedBytesSize(unsigned int input);

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0 };
    unsigned int allocatedBytesSize = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

}
