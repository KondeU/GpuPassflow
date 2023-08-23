#include "backend/BackendContext.h"
#include "BackendWrapper.hpp"

namespace {

static std::unordered_map<au::rhi::BackendContext::Backend,
    std::pair<au::backend::DllWrapper, au::rhi::BackendContext*>> g_storages;

static std::unordered_map<au::gp::ErrorHandler::Instance,
    au::gp::ErrorHandler::Callback> g_loggers;

}

namespace au::rhi {

BackendContext* BackendContext::CreateBackend(Backend type)
{
    static const std::unordered_map<Backend, std::string> libraries = {
        { Backend::DX12,       "backend_dx12"   },
        { Backend::Vulkan,     "backend_vulkan" },
        { Backend::SoftRaster, "backend_cpu"    }
    };

    if (g_storages.find(type) != g_storages.end()) {
        return nullptr;
    }

    auto library = libraries.find(type);
    if (library == libraries.end()) {
        return nullptr;
    }

    auto& instance = g_storages[type]; // backend instance(library wrapper and context pointer)

    if (!instance.first.Load(library->second)) {
        return nullptr;
    }

    instance.second = instance.first.ExecuteFunction<BackendContext*()>("CreateBackend");
    return instance.second;
}

void BackendContext::DestroyBackend(Backend type)
{
    auto backend = g_storages.find(type);
    if (backend == g_storages.end()) {
        return;
    }

    auto& instance = backend->second;

    instance.first.ExecuteFunction<void(BackendContext*)>("DestroyBackend", instance.second);
    instance.first.Unload();

    g_storages.erase(backend);
}

}

namespace au::gp {

ErrorHandler::Instance ErrorHandler::RegisterHandler(Callback callback)
{
    return (g_loggers[callback] = callback);
}

bool ErrorHandler::UnregisterHandler(Instance instance)
{
    return (g_loggers.erase(instance) > 0);
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
    for (const auto& [instance, callback] : g_loggers) {
        if (callback(CallbackParams) != 0) {
            break;
        }
    }
}

}
