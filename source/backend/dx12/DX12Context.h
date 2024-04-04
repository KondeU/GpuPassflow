#pragma once

#include "DX12BackendHeaders.h"
#include "DX12Device.h"

namespace au::backend {

class DX12Context : public rhi::BackendContext
    , DX12Object<DX12Context> {
public:
    explicit DX12Context();
    ~DX12Context() override;

    rhi::Device* CreateDevice(rhi::Device::Description description) override;
    bool DestroyDevice(rhi::Device* device) override;

    std::vector<std::string> GetAvailableDevices() const override;

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    std::vector<std::unique_ptr<DX12Device>> devices;
};

}
