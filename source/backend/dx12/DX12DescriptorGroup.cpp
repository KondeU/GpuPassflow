#include "DX12DescriptorGroup.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12DescriptorGroup::DX12DescriptorGroup(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12DescriptorGroup::~DX12DescriptorGroup()
{
    Shutdown();
}

void DX12DescriptorGroup::Setup(Description description)
{
    this->description = description;
}

void DX12DescriptorGroup::Shutdown()
{
    description = { 0u };
    parameters.clear();
    descriptorRanges.clear();
}

void DX12DescriptorGroup::AddDescriptor(
    DescriptorType type, unsigned int id, ShaderStage visibility)
{
    AddDescriptors(type, { id, id }, visibility);
}

void DX12DescriptorGroup::AddDescriptors(
    DescriptorType type, std::pair<unsigned int, unsigned int> range, ShaderStage visibility)
{
    unsigned int beginId = range.first;
    unsigned int endId = range.second;
    if (beginId > endId) {
        GP_LOG_RET_F(TAG, "Add ranged descriptors information to descriptor group failed! "
            "Arguments are invalid, failed because the begin id is large then the end id.");
    }

    bool isValidDescriptorType = false;
    D3D12_DESCRIPTOR_RANGE_TYPE descriptorRangeType =
        ConvertDescriptorRangeType(type, isValidDescriptorType);

    if (!isValidDescriptorType) {
        GP_LOG_RET_F(TAG, "Add descriptor information to descriptor group failed! "
            "Invalid descriptor type, only can be the buffer/texture/sampler types.");
    }

    descriptorRanges.emplace_back(std::make_unique<CD3DX12_DESCRIPTOR_RANGE>());
    descriptorRanges.back()->Init(descriptorRangeType,
        endId - beginId + 1, beginId, description.space);

    CD3DX12_ROOT_PARAMETER parameter{};
    parameter.InitAsDescriptorTable(1, descriptorRanges.back().get(),
        ConvertShaderVisibility(visibility));

    parameters.emplace_back(parameter);
}

const std::vector<CD3DX12_ROOT_PARAMETER>& DX12DescriptorGroup::GetRootParameters() const
{
    return parameters;
}

}
