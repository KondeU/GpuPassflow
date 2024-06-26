#include "DX12Shader.h"
#include <d3dcompiler.h>

namespace au::backend {

DX12Shader::DX12Shader()
{
}

DX12Shader::~DX12Shader()
{
    Shutdown();
}

void DX12Shader::Setup(Description description)
{
    this->description = description;

    switch (description.sourceType) {
    case Description::SourceType::Source:       ProcessSource(false);   break;
    case Description::SourceType::SourceFile:   ProcessSource(true);    break;
    case Description::SourceType::Bytecode:     ProcessBytecode(false); break;
    case Description::SourceType::BytecodeFile: ProcessBytecode(true);  break;
    }
}

void DX12Shader::Shutdown()
{
    description = { rhi::ShaderStage::Vertex, "" };
    bytecode.Reset();
}

bool DX12Shader::IsValid() const
{
    return (bytecode != nullptr);
}

rhi::Shader::Reflection DX12Shader::Reflect() const
{
    Reflection reflection;

    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> dxReflection;
    LogIfFailedW(D3DReflect(bytecode->GetBufferPointer(),
        bytecode->GetBufferSize(), IID_PPV_ARGS(&dxReflection)));

    D3D12_SHADER_DESC dxShaderDesc{};
    LogIfFailedW(dxReflection->GetDesc(&dxShaderDesc));

    // TODO: Shader reflection to construct PSO

    return reflection;
}

std::string DX12Shader::DumpBytecode() const
{
    if (bytecode != nullptr) {
        return std::string(static_cast<const char*>(
            bytecode->GetBufferPointer()), bytecode->GetBufferSize());
    } else {
        GP_LOG_W(TAG, "Dumping shader bytecode failed, shader not compiled!");
    }
    return std::string();
}

Microsoft::WRL::ComPtr<ID3DBlob> DX12Shader::GetBytecode()
{
    return bytecode;
}

void DX12Shader::ProcessSource(bool fromFile)
{
    UINT compileFlags = 0;
    #if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #endif

    std::string target = "_5_1";
    switch (description.stage) {
    case rhi::ShaderStage::Vertex:   target = "vs" + target; break;
    case rhi::ShaderStage::Hull:     target = "hs" + target; break;
    case rhi::ShaderStage::Domain:   target = "ds" + target; break;
    case rhi::ShaderStage::Geometry: target = "gs" + target; break;
    case rhi::ShaderStage::Pixel:    target = "ps" + target; break;
    case rhi::ShaderStage::Compute:  target = "cs" + target; break;
    default: GP_LOG_RET_W(TAG, "Compile HLSL shader source failed! "
                               "No supported shader stage: %d", description.stage);
    }

    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    if (fromFile) {
        LogIfFailedW(D3DCompileFromFile(std::to_wstring(description.source).c_str(),
            NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, description.entryName.c_str(),
            target.c_str(), compileFlags, 0, &bytecode, &errors));
    } else {
        LogIfFailedW(D3DCompile(description.source.c_str(), description.source.size(),
            NULL, NULL, NULL/* NO SUPPORT #include */, description.entryName.c_str(),
            target.c_str(), compileFlags, 0, &bytecode, &errors));
    }
    if (errors != nullptr) {
        GP_LOG_W(TAG, "Compile HLSL shader logs:\nsource:\n%s\nerror:\n%s",
            fromFile ? description.source.c_str() : "[ from string ]",
            errors->GetBufferPointer());
    }
}

void DX12Shader::ProcessBytecode(bool fromFile)
{
    if (fromFile) {
        LogIfFailedE(D3DReadFileToBlob(
            std::to_wstring(description.source).c_str(), &bytecode));
    } else {
        size_t bytecodeSize = description.source.size();
        LogIfFailedE(D3DCreateBlob(bytecodeSize, &bytecode));
        CopyMemory(bytecode->GetBufferPointer(), description.source.data(), bytecodeSize);
    }
}

}
