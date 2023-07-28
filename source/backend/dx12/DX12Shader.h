#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Shader : public Shader
    , DX12Object<DX12Shader> {
public:
    explicit DX12Shader();
    ~DX12Shader() override;

    void Setup(Description description);
    void Shutdown();

    bool IsValid() const override;

    Reflection Reflect() const override;

    std::string DumpBytecode() const override;

    Microsoft::WRL::ComPtr<ID3DBlob> GetBytecode();

protected:
    void ProcessSource(bool fromFile);
    void ProcessBytecode(bool fromFile);

private:
    Description description{ ShaderStage::Vertex, "" };
    Microsoft::WRL::ComPtr<ID3DBlob> bytecode;
};

}
