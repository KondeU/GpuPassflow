#include "passflow/Passflow.h"

namespace {

static std::mutex g_mutex;
static std::unordered_map<au::gp::Passflow*, au::rhi::BackendContext::Backend> g_passflows;
static std::unordered_map<au::rhi::BackendContext::Backend, au::rhi::BackendContext*> g_contexts;

}

namespace au::gp {

Passflow::Passflow(const std::string& name,
    rhi::BackendContext::Backend backend,
    unsigned int multiBufferingCount)
    : passflowName(name)
    , multipleBufferingCount(multiBufferingCount)
{
    GP_LOG_I(TAG, "Passflow `%s` constructing.", passflowName.c_str());

    rhi::BackendContext* bkContext = nullptr;
    {
        std::lock_guard<std::mutex> locker(g_mutex);
        g_passflows[this] = backend;
        if (!g_contexts[backend]) {
            g_contexts[backend] = rhi::BackendContext::CreateBackend(backend);
        }
        bkContext = g_contexts[backend];
    }
    if (!bkContext) {
        GP_LOG_F(TAG, "Invalid backend context when passflow constructing!");
    }

    commandRecorderNames.resize(multipleBufferingCount);
    for (unsigned int n = 0; n < multipleBufferingCount; n++) {
        commandRecorderNames[n] = passflowName + "." + std::to_string(n);
    }

    bkDevice = bkContext->CreateDevice({ "" /* default adaptor */ });
    bkCommands.resize(multipleBufferingCount);
    for (unsigned int n = 0; n < multipleBufferingCount; n++) {
        bkCommands[n] = bkDevice->CreateCommandRecorder({
            commandRecorderNames[n], rhi::CommandType::Graphics });
    }

    GP_LOG_I(TAG, "Passflow `%s` constructed.", passflowName.c_str());
}

Passflow::~Passflow()
{
    GP_LOG_I(TAG, "Passflow `%s` destructing.", passflowName.c_str());

    bkDevice->WaitIdle();

    for (const auto& name : commandRecorderNames) {
        bkDevice->ReleaseCommandRecordersMemory(name);
    }

    for (auto recorder : bkCommands) {
        bkDevice->DestroyCommandRecorder(recorder);
    }

    {
        std::lock_guard<std::mutex> locker(g_mutex);
        g_passflows.erase(this);
        if (g_passflows.empty()) {
            for (const auto& [backend, context] : g_contexts) {
                if (!context) {
                    GP_LOG_W(TAG, "Invalid backend context when passflow destructing!");
                }
                rhi::BackendContext::DestroyBackend(backend);
            }
            g_contexts.clear();
        }
    }

    GP_LOG_I(TAG, "Passflow `%s` destructed.", passflowName.c_str());
}

unsigned int Passflow::AddPassToFlow(BasePass* pass)
{
    passflow.emplace_back(std::make_pair(pass, false));
    auto index = static_cast<unsigned int>(passflow.size() - 1);

    pass->OnPreparePass(bkDevice);
    pass->OnEnablePass(false);

    return index;
}

BasePass* Passflow::GetPassFromFlow(unsigned int index)
{
    if (index < passflow.size()) {
        return passflow[index].first;
    }
    return nullptr;
}

bool Passflow::EnablePass(unsigned int index, bool enable)
{
    if (index < passflow.size()) {
        passflow[index].second = enable;
        passflow[index].first->OnEnablePass(enable);
        return true;
    }
    return false;
}

bool Passflow::IsEnablePass(unsigned int index)
{
    if (index < passflow.size()) {
        return passflow[index].second;
    }
    return false;
}

unsigned int Passflow::ExecuteWorkflow()
{
    bkCommands[currentBufferingIndex]->Wait();
    bkDevice->ReleaseCommandRecordersMemory(commandRecorderNames[currentBufferingIndex]);

    for (const auto& [pass, enable] : passflow) {
        if (enable) {
            pass->OnBeforePass(currentBufferingIndex);
        }
    }

    bkCommands[currentBufferingIndex]->BeginRecord();

    for (const auto& [pass, enable] : passflow) {
        if (enable) {
            pass->OnExecutePass(bkCommands[currentBufferingIndex]);
        }
    }

    bkCommands[currentBufferingIndex]->EndRecord();
    bkCommands[currentBufferingIndex]->Submit();

    for (unsigned int index = 0; index < passflow.size(); index++) {
        if (passflow[index].second) {
            passflow[index].first->OnAfterPass(index);
        }
    }

    currentBufferingIndex = (currentBufferingIndex + 1) % multipleBufferingCount;
    return currentBufferingIndex; // Return next frame index.
}

}
