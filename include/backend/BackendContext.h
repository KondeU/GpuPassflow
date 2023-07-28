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

namespace au::backend {

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

protected:
    BackendContext() = default;
    virtual ~BackendContext() = default;
};

}
