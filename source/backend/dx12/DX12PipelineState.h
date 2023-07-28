#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;
class DX12PipelineLayout;

class DX12PipelineState : public PipelineState
    , DX12Object<DX12PipelineState> {
public:
    explicit DX12PipelineState(DX12Device& device);
    ~DX12PipelineState() override;

    void Setup(Description description);
    void Shutdown();

    void SetPipelineLayout(PipelineLayout* layout) override;

    void SetIndexAssembly(InputIndexAttribute* iia) override;
    void SetVertexAssembly(InputVertexAttributes* iva) override;

    void SetShader(ShaderStage stage, Shader* shader) override;

    void SetColorOutputFormat(unsigned int location, BasicFormat format) override;
    void SetDepthStencilOutputFormat(BasicFormat format) override;

    void SetRasterizerState(RasterizerState state) override;
    void SetRasterizerStateFillMode(FillMode mode) override;
    void SetRasterizerStateCullMode(CullMode mode) override;
    void SetMSAA(MSAA msaa) override;

    void BuildState() override;

    bool IsItGraphicsPipelineState() const;
    bool IsItComputePipelineState() const;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO();

    DX12PipelineLayout* BindedPipelineLayout() const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ ShaderStage::Graphics };
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineState;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineState;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject; // PSO

    DX12PipelineLayout* pLayout = nullptr;
};

}
