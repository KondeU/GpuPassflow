#pragma once

#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace au::backend {

class DX12Device;

class DX12DescriptorGroup : public DescriptorGroup
    , DX12Object<DX12DescriptorGroup> {
public:
    explicit DX12DescriptorGroup(DX12Device& device);
    ~DX12DescriptorGroup() override;

    void Setup(Description description);
    void Shutdown();

    void AddDescriptor(DescriptorType type,
        unsigned int id, ShaderStage visibility) override;

    void AddDescriptors(DescriptorType type,
        std::pair<unsigned int, unsigned int> range, ShaderStage visibility) override;

    const std::vector<CD3DX12_ROOT_PARAMETER>& GetRootParameters() const;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u };
    // AddDescriptor will add descriptor placeholder to root parameter directly,
    // AddDescriptors will use the descriptor range and add range to root parameter.
    std::vector<CD3DX12_ROOT_PARAMETER> parameters;
    // NB: The descriptorRanges use the unique_ptr to make sure that the raw pointer
    //     of the object in the vector will not change.
    std::vector<std::unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> descriptorRanges;
};

}
