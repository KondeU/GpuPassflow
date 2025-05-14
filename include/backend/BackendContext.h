#pragma once

#ifdef _MSC_VER
  #define BackendApiExport __declspec(dllexport)
  #define BackendApiImport __declspec(dllimport)
#else
  #define BackendApiExport __attribute__((visibility("default")))
  #define BackendApiImport __attribute__((visibility("default")))
#endif

#ifdef BackendModule
  #define BackendApi BackendApiExport
#else
  #define BackendApi BackendApiImport
#endif

#include "Device.h"

namespace au::rhi {

class BackendContext {
public:
    enum class Backend {
        DX12,
        Vulkan
    };

    BackendApi static BackendContext* CreateBackend(Backend type);
    BackendApi static void DestroyBackend(Backend type);

    virtual std::vector<std::string> GetAvailableAdaptors() const = 0;
    virtual Device* CreateDevice(Device::Description description) = 0;
    virtual bool DestroyDevice(Device* device) = 0;

protected:
    BackendContext() = default;
    virtual ~BackendContext() = default;
};

}

namespace au::gp {

class ErrorHandler final {
public:
    using Instance = void*; // value is equal to callback parameter.
    using Callback = int(*)(const char* [3]); // level, tag, content.

    BackendApi static Instance RegisterHandler(Callback callback);
    BackendApi static bool UnregisterHandler(Instance instance);

    BackendApi static void Logging(const char* level, const char* tag, const char* format, ...);

private:
    ErrorHandler() = delete;
};

}

#define GP_LOG_TAG(name) static constexpr char* TAG = #name
#define GP_LOG_D(tag, format, ...) au::gp::ErrorHandler::Logging("D", tag, format, ##__VA_ARGS__)
#define GP_LOG_I(tag, format, ...) au::gp::ErrorHandler::Logging("I", tag, format, ##__VA_ARGS__)
#define GP_LOG_W(tag, format, ...) au::gp::ErrorHandler::Logging("W", tag, format, ##__VA_ARGS__)
#define GP_LOG_E(tag, format, ...) au::gp::ErrorHandler::Logging("E", tag, format, ##__VA_ARGS__)
#define GP_LOG_F(tag, format, ...) au::gp::ErrorHandler::Logging("F", tag, format, ##__VA_ARGS__)
