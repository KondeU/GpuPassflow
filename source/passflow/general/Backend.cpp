#include "Backend.h"
#include "Configuration.h"

namespace au::passflow {

backend::Device* Backend::Device()
{
    if (!device) {
        if (!context) {
            backend = Configuration::GetReference().GetBackendType();
            if (!(context = backend::BackendContext::CreateBackend(backend))) {
                AU_LOG_RETN_F(TAG, "Create backend context failed!");
            }
            AU_LOG_I(TAG, "Create backend context finished.");
        }
        if (!(device = context->CreateDevice({ "" /* default adaptor */ }))) {
            AU_LOG_RETN_F(TAG, "Create backend device failed!");
        }
        AU_LOG_I(TAG, "Create backend device finished.");
    }
    return device;
}

bool Backend::Close()
{
    if (device) {
        if (!context->DestroyDevice(device)) {
            AU_LOG_RETF_E(TAG, "Destroy backend device failed!");
        }
        device = nullptr;
        AU_LOG_I(TAG, "Destroy backend device finished.");
    }
    if (context) {
        context = nullptr;
        backend::BackendContext::DestroyBackend(backend);
        AU_LOG_I(TAG, "Destroy backend context finished.");
    }
    return true;
}

}
