#include "DX12ImageSampler.h"
#include "DX12BasicTypes.h"

namespace au::backend {

DX12ImageSampler::DX12ImageSampler()
{
}

DX12ImageSampler::~DX12ImageSampler()
{
    Shutdown();
}

void DX12ImageSampler::Setup(Description description)
{
    samplerState = ConvertSamplerState(description.state);
}

void DX12ImageSampler::Shutdown()
{
    ZeroMemory(&samplerState, sizeof(samplerState));
}

const D3D12_SAMPLER_DESC& DX12ImageSampler::NativeSamplerState() const
{
    return samplerState;
}

}
