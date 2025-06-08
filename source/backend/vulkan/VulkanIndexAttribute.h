#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanIndexAttribute : public rhi::IndexAttribute
    , VulkanObject<VulkanIndexAttribute> {
public:
    VulkanIndexAttribute();
    ~VulkanIndexAttribute() override;
};

} 