#include "backend/BackendContext.h"
#include "BackendWrapper.hpp"

namespace {

static std::unordered_map<au::rhi::BackendContext::Backend,
    std::pair<au::backend::DllWrapper, au::rhi::BackendContext*>> g_storages;

static std::vector<au::gp::ErrorHandler::Callback> g_loggers;

GP_LOG_TAG("BackendContext");

}

namespace au::rhi {

BackendContext* BackendContext::CreateBackend(Backend type)
{
    static const std::unordered_map<Backend, std::string> libraries = {
        { Backend::DX12,       "backend_dx12"   },
        { Backend::Vulkan,     "backend_vulkan" },
        { Backend::SoftRaster, "backend_cpu"    }
    };

    GP_LOG_I(TAG, "Create backend: %d", gp::EnumCast(type));

    if (g_storages.find(type) != g_storages.end()) {
        GP_LOG_F(TAG, "Create backend failed, it is already in the backend storage.");
        return nullptr;
    }

    auto library = libraries.find(type);
    if (library == libraries.end()) {
        GP_LOG_F(TAG, "Create backend failed, no matching dynamic library.");
        return nullptr;
    }

    auto& instance = g_storages[type]; // backend instance(library wrapper and context pointer)

    if (!instance.first.Load(library->second)) {
        GP_LOG_F(TAG, "Create backend failed, load dynamic library failed.");
        return nullptr;
    }

    instance.second = instance.first.ExecuteFunction<BackendContext*()>("CreateBackend");
    return instance.second;
}

void BackendContext::DestroyBackend(Backend type)
{
    GP_LOG_I(TAG, "Destroy backend: %d", gp::EnumCast(type));

    auto backend = g_storages.find(type);
    if (backend == g_storages.end()) {
        GP_LOG_E(TAG, "Destroy backend failed, not found in the backend storage.");
        return;
    }

    auto& instance = backend->second;

    instance.first.ExecuteFunction<void(BackendContext*)>("DestroyBackend", instance.second);
    if (!instance.first.Unload()) {
        GP_LOG_W(TAG, "Destroy backend but unload dynamic library failed.");
    }

    g_storages.erase(backend);
}

}

namespace au::gp {

ErrorHandler::Instance ErrorHandler::RegisterHandler(Callback callback)
{
    ErrorHandler::Instance instance = g_loggers.emplace_back(callback);
    GP_LOG_I(TAG, "Register error handler: %p", instance);
    return instance;
}

bool ErrorHandler::UnregisterHandler(Instance instance)
{
    GP_LOG_I(TAG, "Unregister error handler: %p", instance);
    auto iter = std::find(g_loggers.begin(), g_loggers.end(), instance);
    if (iter == g_loggers.end()) {
        GP_LOG_W(TAG, "Unregister error handler failed, instance not found.");
        return false;
    }
    g_loggers.erase(iter);
    return true;
}

void ErrorHandler::Logging(const char* level, const char* tag, const char* format, ...)
{
    constexpr size_t ContentSize = 2048; // 2KB, half of a page.
    char content[ContentSize]{};

    va_list args{};
    va_start(args, format);
    vsnprintf(content, ContentSize, format, args);
    va_end(args);

    // Ensuring that access to character arrays is safe.
    if (content[ContentSize - 4] != '\0') {
        content[ContentSize - 4] = '.';
        content[ContentSize - 3] = '.';
        content[ContentSize - 2] = '.';
        content[ContentSize - 1] = '\0';
    }

    const char* CallbackParams[3] = { level, tag, content };
    for (const auto& callback : g_loggers) {
        if (callback(CallbackParams) != 0) {
            break;
        }
    }
}

}
