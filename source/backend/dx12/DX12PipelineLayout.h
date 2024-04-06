#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;

class DX12PipelineLayout : public rhi::PipelineLayout
    , DX12Object<DX12PipelineLayout> {
public:
    explicit DX12PipelineLayout(DX12Device& device);
    ~DX12PipelineLayout() override;

    void Setup(Description description);
    void Shutdown();

    bool AddGroup(rhi::DescriptorGroup* group) override;
    bool BuildLayout() override;

    bool IsValid() const override;

    std::string DumpCache() const override;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> Signature();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description;
    std::vector<CD3DX12_ROOT_PARAMETER> parameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature;
    Microsoft::WRL::ComPtr<ID3DBlob> serializeRootSignatureError;
};

}
