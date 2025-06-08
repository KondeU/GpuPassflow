#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanVertexAttribute : public rhi::VertexAttribute
    , VulkanObject<VulkanVertexAttribute> {
public:
    VulkanVertexAttribute();
    ~VulkanVertexAttribute() override;
};

} 