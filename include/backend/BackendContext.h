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
        Vulkan,
        SoftRaster
    };

    BackendApi static BackendContext* CreateBackend(Backend type);
    BackendApi static void DestroyBackend(Backend type);

    virtual Device* CreateDevice(Device::Description description) = 0;
    virtual bool DestroyDevice(Device* device) = 0;

    virtual std::vector<std::string> GetAvailableAdaptors() const = 0;

protected:
    BackendContext() = default;
    virtual ~BackendContext() = default;
};

}

namespace au::gp {

class ErrorHandler final {
public:
    using Instance = void*; // value is equal to callback yet.
    using Callback = int(*)(const char* [3]); // level, tag, content.

    BackendApi static Instance RegisterHandler(Callback callback);
    BackendApi static bool UnregisterHandler(Instance instance);

    BackendApi static void Logging(const char* level, const char* tag, const char* format, ...);

private:
    ErrorHandler() = delete;
};

}

#define GP_LOG_D(tag, format, ...) \
au::gp::ErrorHandler::Logging("D", tag, format, ##__VA_ARGS__)
#define GP_LOG_I(tag, format, ...) \
au::gp::ErrorHandler::Logging("I", tag, format, ##__VA_ARGS__)
#define GP_LOG_W(tag, format, ...) \
au::gp::ErrorHandler::Logging("W", tag, format, ##__VA_ARGS__)
#define GP_LOG_E(tag, format, ...) \
au::gp::ErrorHandler::Logging("E", tag, format, ##__VA_ARGS__)
#define GP_LOG_F(tag, format, ...) \
au::gp::ErrorHandler::Logging("F", tag, format, ##__VA_ARGS__)

#define GP_LOG_RETN_D(tag, format, ...) \
do { GP_LOG_D(tag, format, ##__VA_ARGS__); return nullptr; } while(0)
#define GP_LOG_RETN_I(tag, format, ...) \
do { GP_LOG_I(tag, format, ##__VA_ARGS__); return nullptr; } while(0)
#define GP_LOG_RETN_W(tag, format, ...) \
do { GP_LOG_W(tag, format, ##__VA_ARGS__); return nullptr; } while(0)
#define GP_LOG_RETN_E(tag, format, ...) \
do { GP_LOG_E(tag, format, ##__VA_ARGS__); return nullptr; } while(0)
#define GP_LOG_RETN_F(tag, format, ...) \
do { GP_LOG_F(tag, format, ##__VA_ARGS__); return nullptr; } while(0)

#define GP_LOG_RETF_D(tag, format, ...) \
do { GP_LOG_D(tag, format, ##__VA_ARGS__); return false; } while(0)
#define GP_LOG_RETF_I(tag, format, ...) \
do { GP_LOG_I(tag, format, ##__VA_ARGS__); return false; } while(0)
#define GP_LOG_RETF_W(tag, format, ...) \
do { GP_LOG_W(tag, format, ##__VA_ARGS__); return false; } while(0)
#define GP_LOG_RETF_E(tag, format, ...) \
do { GP_LOG_E(tag, format, ##__VA_ARGS__); return false; } while(0)
#define GP_LOG_RETF_F(tag, format, ...) \
do { GP_LOG_F(tag, format, ##__VA_ARGS__); return false; } while(0)

#define GP_LOG_RETD_D(tag, format, ...) \
do { GP_LOG_D(tag, format, ##__VA_ARGS__); return {}; } while(0)
#define GP_LOG_RETD_I(tag, format, ...) \
do { GP_LOG_I(tag, format, ##__VA_ARGS__); return {}; } while(0)
#define GP_LOG_RETD_W(tag, format, ...) \
do { GP_LOG_W(tag, format, ##__VA_ARGS__); return {}; } while(0)
#define GP_LOG_RETD_E(tag, format, ...) \
do { GP_LOG_E(tag, format, ##__VA_ARGS__); return {}; } while(0)
#define GP_LOG_RETD_F(tag, format, ...) \
do { GP_LOG_F(tag, format, ##__VA_ARGS__); return {}; } while(0)

#define GP_LOG_RET_D(tag, format, ...) \
do { GP_LOG_D(tag, format, ##__VA_ARGS__); return; } while(0)
#define GP_LOG_RET_I(tag, format, ...) \
do { GP_LOG_I(tag, format, ##__VA_ARGS__); return; } while(0)
#define GP_LOG_RET_W(tag, format, ...) \
do { GP_LOG_W(tag, format, ##__VA_ARGS__); return; } while(0)
#define GP_LOG_RET_E(tag, format, ...) \
do { GP_LOG_E(tag, format, ##__VA_ARGS__); return; } while(0)
#define GP_LOG_RET_F(tag, format, ...) \
do { GP_LOG_F(tag, format, ##__VA_ARGS__); return; } while(0)

#define GP_LOG_TAG(name) static constexpr char* TAG = #name
