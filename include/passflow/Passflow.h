#pragma once

#include "RasterizePass.h"
#include "ComputePass.h"

namespace au::gp {

class Passflow {
public:
    explicit Passflow(const std::string& name,
        rhi::BackendContext::Backend backend =
        #ifdef WIN32
        rhi::BackendContext::Backend::DX12,
        #else
        rhi::BackendContext::Backend::Vulkan,
        #endif
        unsigned int multiBufferingCount = 3);
    virtual ~Passflow();

    template <typename Pass, class ...Args>
    Pass* CreateOrGetPass(const std::string& name, const Args& ...args)
    {
        static_assert(std::is_base_of<BasePass, Pass>(), "Pass should inherit from BasePass!");
        if (auto it = passes.find(name); it != passes.end()) {
            return CastPass<Pass>(it->second.get());
        }
        return CastPass<Pass>((passes[name] = std::make_unique<Pass>(*this, args...)).get());
    }

    template <typename Pass>
    Pass* CastPass(BasePass* pass) const
    {
        static_assert(std::is_base_of<BasePass, Pass>(), "Pass should inherit from BasePass!");
        return dynamic_cast<Pass*>(pass);
    }

    unsigned int AddPassToFlow(BasePass* pass);
    BasePass* GetPassFromFlow(unsigned int index);
    bool EnablePass(unsigned int index, bool enable);
    bool IsEnablePass(unsigned int index);

    unsigned int ExecuteWorkflow();

    template <typename T, class ...Args>
    Resource<T> MakeResource(Args&& ...args)
    {
        return std::make_shared<T>(std::forward(args)...);
    }

private:
    GP_LOG_TAG(Passflow);

    std::string passflowName;

    const unsigned int multipleBufferingCount;
    unsigned int currentBufferingIndex = 0;

    std::unordered_map<std::string, std::unique_ptr<BasePass>> passes;
    std::vector<std::pair<BasePass*, bool>> passflow;

    rhi::Device* bkDevice = nullptr;
    std::vector<rhi::CommandRecorder*> bkCommands;
    std::vector<std::string> commandRecorderNames;
};

}
