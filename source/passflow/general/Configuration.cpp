#include "Configuration.h"

namespace au::gp {

bool Configuration::SetBackendType(backend::BackendContext::Backend type)
{
    if (backendTypeGot) {
        GP_LOG_RETF_E(TAG, "Set backend type failed, it has been got.");
    }
    switch (type) {
    case backend::BackendContext::Backend::DX12:
        GP_LOG_I(TAG, "Set backend type: DX12.");
        break;
    case backend::BackendContext::Backend::Vulkan:
        GP_LOG_I(TAG, "Set backend type: Vulkan.");
        break;
    case backend::BackendContext::Backend::SoftRaster:
        GP_LOG_I(TAG, "Set backend type: SoftRaster.");
        break;
    }
    backendType = type;
    return true;
}

backend::BackendContext::Backend Configuration::GetBackendType() const
{
    if (!backendTypeGot) {
        switch (backendType) {
        case backend::BackendContext::Backend::DX12:
            GP_LOG_I(TAG, "Backend type is DX12, it is initialized and cannot be modified.");
            break;
        case backend::BackendContext::Backend::Vulkan:
            GP_LOG_I(TAG, "Backend type is Vulkan, it is initialized and cannot be modified.");
            break;
        case backend::BackendContext::Backend::SoftRaster:
            GP_LOG_I(TAG, "Backend type is SoftRaster, it is initialized and cannot be modified.");
            break;
        }
        backendTypeGot = true;
    }
    return backendType;
}

bool Configuration::SetMultipleBufferingCount(unsigned int count)
{
    if (multipleBufferingCountGot) {
        GP_LOG_RETF_E(TAG, "Set multiple buffering count failed, it has been got.");
    }
    GP_LOG_I(TAG, "Set multiple buffering count: %d.", count);
    multipleBufferingCount = count;
    return true;
}

unsigned int Configuration::GetMultipleBufferingCount() const
{
    if (!multipleBufferingCountGot) {
        GP_LOG_I(TAG, "Multiple buffering count is %d, "
            "it is initialized and cannot be modified.",
            multipleBufferingCount);
        multipleBufferingCountGot = true;
    }
    return multipleBufferingCount;
}

}
