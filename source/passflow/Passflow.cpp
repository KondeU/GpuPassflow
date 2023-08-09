#include "passflow/Passflow.h"
#include "general/Configuration.h"
#include "general/Backend.h"

namespace au::passflow {

Passflow::Passflow(std::string name) : passflowName(name)
{
    multipleBufferingCount = Configuration::GetReference().GetMultipleBufferingCount();

    commandRecorderNames.resize(multipleBufferingCount);
    for (unsigned int n = 0; n < multipleBufferingCount; n++) {
        commandRecorderNames[n] = passflowName + "." + std::to_string(n);
    }

    bkDevice = Backend::GetReference().Device();
    bkCommands.resize(multipleBufferingCount);
    for (unsigned int n = 0; n < multipleBufferingCount; n++) {
        bkCommands[n] = bkDevice->CreateCommandRecorder({
            commandRecorderNames[n], CommandType::Graphics });
    }

    passflowsCount++;
    GP_LOG_I(TAG, "Passflow `%s` construction.", passflowName.c_str());
}

Passflow::~Passflow()
{
    bkDevice->WaitIdle();

    for (const auto& name : commandRecorderNames) {
        bkDevice->ReleaseCommandRecordersMemory(name);
    }

    for (auto recorder : bkCommands) {
        bkDevice->DestroyCommandRecorder(recorder);
    }

    passflowsCount--;
    GP_LOG_I(TAG, "Passflow `%s` destruction.", passflowName.c_str());
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

bool Passflow::CloseBackend()
{
    if (passflowsCount > 0) {
        GP_LOG_RETF_W(TAG, "Cannot close the backend because passflows count is not zero! "
            "Passflows count is `%d`, there maybe some passflows are using GPU.", passflowsCount);
    }
    return Backend::GetReference().Close();
}

unsigned int Passflow::passflowsCount = 0;

}
