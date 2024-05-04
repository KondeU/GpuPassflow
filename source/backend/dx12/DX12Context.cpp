#include "DX12Context.h"
#include "DX12Common.h"

#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h> // IDXGIInfoQueue
#endif

namespace au::backend {

DX12Context::DX12Context()
{
    UINT dxgiFactoryFlags = 0;

    #if defined(DEBUG) || defined(_DEBUG)
    GP_LOG_I(TAG, "Enable DX12 debug layer.");
    Microsoft::WRL::ComPtr<ID3D12Debug> debugger;
    LogIfFailedD(D3D12GetDebugInterface(IID_PPV_ARGS(&debugger)));
    if (debugger != nullptr) {
        debugger->EnableDebugLayer();
    }
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
        dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        // DX12 does not support redirect debug layer message currently, see:
        // https://devblogs.microsoft.com/directx/d3d12-debug-layer-message-callback/
        // Sample code is:
        // ComPtr<IDXGIInfoQueue1> dxgiInfoQueue;
        // dxgiInfoQueue->RegisterMessageCallback(..);
    }
    #endif

    LogIfFailedF(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgi)));
}

DX12Context::~DX12Context()
{
    devices.resize(0);
    if (dxgi.Reset() > 0) {
        GP_LOG_E(TAG, "dxgi leak!");
    }
}

rhi::Device* DX12Context::CreateDevice(rhi::Device::Description description)
{
    return CreateInstance<rhi::Device>(devices, description, dxgi);
}

bool DX12Context::DestroyDevice(rhi::Device* device)
{
    return DestroyInstance(devices, device);
}

std::vector<std::string> DX12Context::GetAvailableAdaptors() const
{
    std::vector<std::string> adaptorsList;

    GP_LOG_D(TAG, "Enum adapters...");
    std::vector<IDXGIAdapter*> adapters;

    GP_LOG_D(TAG, "Adapters:");
    IDXGIAdapter* adapter = nullptr;
    for (UINT i = 0; dxgi->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        GP_LOG_D(TAG, "> %d : %s", i, std::to_string(desc.Description).c_str());
        adaptorsList.emplace_back(std::to_string(desc.Description).c_str());
        adapters.emplace_back(adapter);
    }

    GP_LOG_D(TAG, "Enum each adapter outputs...");
    for (size_t n = 0; n < adapters.size(); n++) {
        GP_LOG_D(TAG, "Adapter %d Outputs:", n);

        // Adaptor output: usually is a displayer(monitor).
        size_t outputsCount = 0;
        IDXGIOutput* output = nullptr;
        for (UINT i = 0; adapters[n]->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; i++) {
            DXGI_OUTPUT_DESC desc;
            output->GetDesc(&desc);
            GP_LOG_D(TAG, "> %d : %s", i, std::to_string(desc.DeviceName).c_str());

            GP_LOG_D(TAG, "  - OutputDisplayModes");
            {
                // Using default back buffer format to get display mode list.
                constexpr DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
                constexpr UINT flags = 0;

                UINT count = 0;
                output->GetDisplayModeList(format, flags, &count, NULL);

                std::vector<DXGI_MODE_DESC> modes(count);
                output->GetDisplayModeList(format, flags, &count, &modes[0]);

                for (const auto& mode : modes) {
                    UINT n = mode.RefreshRate.Numerator;
                    UINT d = mode.RefreshRate.Denominator;
                    std::string text =
                        "Width = " + std::to_string(mode.Width) + ", " +
                        "Height = " + std::to_string(mode.Height) + ", " +
                        "Refresh = " + std::to_string(n) + "/" + std::to_string(d) +
                        "=" + std::to_string(static_cast<float>(n) / static_cast<float>(d));
                    GP_LOG_D(TAG, "    %s", text.c_str());
                }
            }

            outputsCount++;
            ReleaseCOM(output);
        }

        if (outputsCount == 0) {
            GP_LOG_D(TAG, "Adapter %d has no output.", n);
        }
    }

    for (auto adapter : adapters) {
        ReleaseCOM(adapter);
    }

    return adaptorsList;
}

}
