#include "DX12PipelineLayout.h"
#include "DX12DescriptorGroup.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12PipelineLayout::DX12PipelineLayout(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12PipelineLayout::~DX12PipelineLayout()
{
    Shutdown();
}

void DX12PipelineLayout::Setup(Description description)
{
}

void DX12PipelineLayout::Shutdown()
{
    parameters.clear();
    samplers.clear();
    signature.Reset();
}

void DX12PipelineLayout::AddGroup(DescriptorGroup* group)
{
    const auto& params = dynamic_cast<DX12DescriptorGroup*>(group)->GetRootParameters();
    parameters.insert(parameters.end(), params.begin(), params.end());
}

void DX12PipelineLayout::BuildLayout()
{
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
        static_cast<UINT>(parameters.size()),
        (parameters.size() > 0) ? parameters.data() : nullptr,
        static_cast<UINT>(samplers.size()),
        (samplers.size()   > 0) ? samplers.data() : nullptr,
        // Setting this flag means that current application is opting in to using the Input
        // Assembler (requiring an input layout that defines a set of vertex buffer bindings).
        // Omitting this flag can result in one root argument space being saved on some hardware.
        // It is better to omit this flag if the Input Assembler is not required, though the
        // optimization is minor.
        // see: https://docs.microsoft.com/zh-cn/windows/win32/api/d3d12/ne-d3d12-d3d12_root_signature_flags
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature;
    Microsoft::WRL::ComPtr<ID3DBlob> serializeRootSignatureError;
    LogIfFailedW(D3D12SerializeRootSignature(
        &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSignature, &serializeRootSignatureError));
    if (serializeRootSignatureError != nullptr) {
        GP_LOG_RET_W(TAG, "Serialize root signature failed!\nerror:\n%s",
            serializeRootSignatureError->GetBufferPointer());
    }

    LogIfFailedF(device->CreateRootSignature(0,
        serializedRootSignature->GetBufferPointer(),
        serializedRootSignature->GetBufferSize(),
        IID_PPV_ARGS(&signature)));
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> DX12PipelineLayout::Signature()
{
    return signature;
}

}
