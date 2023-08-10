#pragma once

#include "backend/BackendContext.h"

namespace au::gp {

class Configuration final : public common::GlobalSingleton<Configuration> {
public:
    bool SetBackendType(backend::BackendContext::Backend type);
    backend::BackendContext::Backend GetBackendType() const;

    bool SetMultipleBufferingCount(unsigned int count);
    unsigned int GetMultipleBufferingCount() const;

private:
    GP_LOG_TAG(PassflowConfiguration);

    backend::BackendContext::Backend backendType
    #if WIN32
        = backend::BackendContext::Backend::DX12;
    #elif __linux__
        = backend::BackendContext::Backend::Vulkan;
    #else
        = backend::BackendContext::Backend::SoftRaster;
    #endif
    unsigned int multipleBufferingCount = 3;
    // xxxGot is used to mark whether program has called the GetXxx interface.
    // Once the GetXxx interface is called, the configuration is considered
    // complete and the SetXxx interface will no longer take effect.
    mutable bool backendTypeGot = false;
    mutable bool multipleBufferingCountGot = false;
};

}
