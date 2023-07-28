#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;

class DX12ImageSampler : public ImageSampler
    , DX12Object<DX12ImageSampler> {
public:
    explicit DX12ImageSampler();
    ~DX12ImageSampler() override;

    void Setup(Description description);
    void Shutdown();

    const D3D12_SAMPLER_DESC& NativeSamplerState() const;

private:
    D3D12_SAMPLER_DESC samplerState{};
};

}
