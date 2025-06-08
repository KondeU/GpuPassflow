#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanConstantBuffer : public rhi::ConstantBuffer
    , VulkanObject<VulkanConstantBuffer> {
public:
    VulkanConstantBuffer();
    ~VulkanConstantBuffer() override;
};

} 