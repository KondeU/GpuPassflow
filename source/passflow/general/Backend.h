#pragma once

#include "backend/BackendContext.h"

namespace au::gp {

class Backend final : public common::GlobalSingleton<Backend> {
public:
    backend::Device* Device();
    bool Close();

private:
    GP_LOG_TAG(PassflowBackend);

    backend::BackendContext::Backend backend;
    backend::BackendContext* context = nullptr;
    backend::Device*         device  = nullptr;
};

}
