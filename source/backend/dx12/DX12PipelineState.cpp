#include "DX12PipelineState.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12PipelineState::DX12PipelineState(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12PipelineState::~DX12PipelineState()
{
    Shutdown();
}

void DX12PipelineState::Setup(Description description)
{
    this->description = description;
    ZeroMemory(&computePipelineState, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
    ZeroMemory(&graphicsPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    //--------------------- D3D12_GRAPHICS_PIPELINE_STATE_DESC ---------------------
    // ID3D12RootSignature* pRootSignature;                 - SetPipelineLayout
    // D3D12_SHADER_BYTECODE VS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE PS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE DS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE HS;                            - NULL and 0 / SetShader
    // D3D12_SHADER_BYTECODE GS;                            - NULL and 0 / SetShader
    // D3D12_STREAM_OUTPUT_DESC StreamOutput;               - NULL and 0
    // D3D12_BLEND_DESC BlendState;                         - Default / SetBlendState..
    // UINT SampleMask;                                     - Default: UINT_MAX
    // D3D12_RASTERIZER_DESC RasterizerState;               - Default / SetRasterizerState..
    // D3D12_DEPTH_STENCIL_DESC DepthStencilState;          - Default / SetDepthStencilState..
    // D3D12_INPUT_LAYOUT_DESC InputLayout;                 - Default: NULL & 0 / SetVertexAssembly
    // D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;  - Default: Disable  / SetIndexAssembly
    // D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType; - Default: Triangle / SetIndexAssembly
    // UINT NumRenderTargets;                               - Initial: 0 / BuildState
    // DXGI_FORMAT RTVFormats[8];                     - DXGI_FORMAT_UNKNOWN(0) / SetColorAttachment
    // DXGI_FORMAT DSVFormat;                  - DXGI_FORMAT_UNKNOWN(0) / SetDepthStencilAttachment
    // DXGI_SAMPLE_DESC SampleDesc;            - Default: MSAAx1(1) & 0 / SetMSAA
    // UINT NodeMask;                          - 0
    // D3D12_CACHED_PIPELINE_STATE CachedPSO;  - NULL & 0 / PipelineState::Description::cache
    // D3D12_PIPELINE_STATE_FLAGS Flags;       - 0
    //------------------------------------------------------------------------------
    graphicsPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    graphicsPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    graphicsPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    graphicsPipelineState.SampleMask = UINT_MAX;
    graphicsPipelineState.SampleDesc.Count = 1;
    graphicsPipelineState.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    graphicsPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

void DX12PipelineState::Shutdown()
{
    description = { ShaderStage::Graphics };
    pipelineStateObject.Reset();
    pLayout = nullptr;
}

void DX12PipelineState::SetPipelineLayout(PipelineLayout* layout)
{
    pLayout = dynamic_cast<DX12PipelineLayout*>(layout);
    computePipelineState.pRootSignature = pLayout->Signature().Get();
    graphicsPipelineState.pRootSignature = pLayout->Signature().Get();
}

void DX12PipelineState::SetIndexAssembly(InputIndexAttribute* iia)
{
    auto dxIia = dynamic_cast<DX12InputIndexAttribute*>(iia);
    graphicsPipelineState.IBStripCutValue =
        dxIia->GetIndexInformation().StripCutValue;
    graphicsPipelineState.PrimitiveTopologyType =
        dxIia->GetIndexInformation().PrimitiveTopologyType;
}

void DX12PipelineState::SetVertexAssembly(InputVertexAttributes* iva)
{
    auto dxIva = dynamic_cast<DX12InputVertexAttributes*>(iva);
    graphicsPipelineState.InputLayout = {
        dxIva->GetInputElements().data(),
        static_cast<UINT>(dxIva->GetInputElements().size())
    };
}

void DX12PipelineState::SetShader(ShaderStage stage, Shader* shader)
{
    if (!(framework::EnumCast(stage) & framework::EnumCast(description.enabledStage))) {
        GP_LOG_RET_E(TAG, "Pipeline state set shader failed, stage not enabled.");
    }

    if (!shader || !shader->IsValid()) {
        GP_LOG_RET_E(TAG, "Pipeline state set shader failed, shader is invalid.");
    }

    auto dxShader = dynamic_cast<DX12Shader*>(shader);
    auto bytecodeBlob = dxShader->GetBytecode();
    D3D12_SHADER_BYTECODE bytecode{
        bytecodeBlob->GetBufferPointer(),
        bytecodeBlob->GetBufferSize()
    };

    switch (stage) {
    case ShaderStage::Vertex:   graphicsPipelineState.VS = bytecode; break;
    case ShaderStage::Hull:     graphicsPipelineState.HS = bytecode; break;
    case ShaderStage::Domain:   graphicsPipelineState.DS = bytecode; break;
    case ShaderStage::Geometry: graphicsPipelineState.GS = bytecode; break;
    case ShaderStage::Pixel:    graphicsPipelineState.PS = bytecode; break;
    case ShaderStage::Compute:  computePipelineState.CS  = bytecode; break;
    default: GP_LOG_RET_W(TAG, "Pipeline state set shader failed, invalid stage!");
    }
}

void DX12PipelineState::SetColorOutputFormat(unsigned int location, BasicFormat format)
{
    if (location >= (sizeof(graphicsPipelineState.RTVFormats) /
                     sizeof(graphicsPipelineState.RTVFormats[0]))) {
        GP_LOG_RET_W(TAG, "Pipeline state set color attachment failed, location overflow!");
    }

    graphicsPipelineState.RTVFormats[location] = ConvertBasicFormat(format);
    // Scan all render targets format to detect and update render targets count.
    graphicsPipelineState.NumRenderTargets = 0;
    for (DXGI_FORMAT renderTargetFormat : graphicsPipelineState.RTVFormats) {
        if (renderTargetFormat != DXGI_FORMAT_UNKNOWN) {
            graphicsPipelineState.NumRenderTargets++;
        }
    }
}

void DX12PipelineState::SetDepthStencilOutputFormat(BasicFormat format)
{
    graphicsPipelineState.DSVFormat = ConvertBasicFormat(format);
}

void DX12PipelineState::SetRasterizerState(RasterizerState state)
{
    graphicsPipelineState.RasterizerState = ConvertRasterizerState(state);
}

void DX12PipelineState::SetRasterizerStateFillMode(FillMode mode)
{
    graphicsPipelineState.RasterizerState.FillMode = ConvertFillMode(mode);
}

void DX12PipelineState::SetRasterizerStateCullMode(CullMode mode)
{
    graphicsPipelineState.RasterizerState.CullMode = ConvertCullMode(mode);
}

void DX12PipelineState::SetMSAA(MSAA msaa)
{
    graphicsPipelineState.SampleDesc.Count = ConvertMSAA(msaa);
}

void DX12PipelineState::BuildState()
{
    if (framework::EnumCast(description.enabledStage) &
        framework::EnumCast(ShaderStage::Graphics)) {
        if (framework::EnumCast(description.enabledStage) &
            framework::EnumCast(ShaderStage::Compute)) {
            GP_LOG_RET_E(TAG, "Build pipeline state failed, you can not "
                "enable both Graphics and Compute stage at the same time.");
        }
        LogIfFailedE(device->CreateGraphicsPipelineState(
            &graphicsPipelineState, IID_PPV_ARGS(&pipelineStateObject)));
    } else {
        // Otherwise is ShaderStage::Compute
        LogIfFailedE(device->CreateComputePipelineState(
            &computePipelineState, IID_PPV_ARGS(&pipelineStateObject)));
    }
}

bool DX12PipelineState::IsItGraphicsPipelineState() const
{
    if (framework::EnumCast(description.enabledStage) &
        framework::EnumCast(ShaderStage::Graphics)) {
        if (framework::EnumCast(description.enabledStage) &
            framework::EnumCast(ShaderStage::Compute)) {
            return false;
        }
        return true;
    }
    return false;
}

bool DX12PipelineState::IsItComputePipelineState() const
{
    if (framework::EnumCast(description.enabledStage) &
        framework::EnumCast(ShaderStage::Compute)) {
        if (framework::EnumCast(description.enabledStage) &
            framework::EnumCast(ShaderStage::Graphics)) {
            return false;
        }
        return true;
    }
    return false;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> DX12PipelineState::PSO()
{
    return pipelineStateObject;
}

DX12PipelineLayout* DX12PipelineState::BindedPipelineLayout() const
{
    return pLayout;
}

}
