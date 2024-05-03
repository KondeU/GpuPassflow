#pragma once

namespace au::gp {

class Passflow;

class BasePass {
public:
    virtual ~BasePass() = default;

    virtual void OnPreparePass(rhi::Device* device) = 0;
    virtual void OnBeforePass(unsigned int currentBufferingIndex) = 0;
    virtual void OnExecutePass(rhi::CommandRecorder* recorder) = 0;
    virtual void OnAfterPass(unsigned int currentPassInFlowIndex) = 0;
    virtual void OnEnablePass(bool enable) = 0;

protected:
    explicit BasePass(Passflow& passflow);

    Passflow& passflow;
};

}
