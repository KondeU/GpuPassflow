#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class PipelineLayout;
class InputIndexAttribute;
class InputVertexAttributes;
class Shader;

class PipelineState {
public:
    struct Description {
        ShaderStage enabledStage;

        Description(
            ShaderStage enabledStage)
            : enabledStage(enabledStage)
        {}
    };

    // Resource state
    virtual void SetPipelineLayout(PipelineLayout* layout) = 0;

    // Input state
    virtual void SetIndexAssembly(InputIndexAttribute* iia) = 0;
    virtual void SetVertexAssembly(InputVertexAttributes* iva) = 0;

    // Shader program
    virtual void SetShader(ShaderStage stage, Shader* shader) = 0;

    // Output state
    virtual void SetColorOutputFormat(unsigned int location, BasicFormat format) = 0;
    virtual void SetDepthStencilOutputFormat(BasicFormat format) = 0;

    // Non-programming state
    virtual void SetRasterizerState(RasterizerState state) = 0;
    virtual void SetRasterizerStateFillMode(FillMode mode) = 0;
    virtual void SetRasterizerStateCullMode(CullMode mode) = 0;
    virtual void SetMSAA(MSAA msaa) = 0;

    // Build PSO
    virtual void BuildState() = 0;

protected:
    PipelineState() = default;
    virtual ~PipelineState() = default;
};

}
