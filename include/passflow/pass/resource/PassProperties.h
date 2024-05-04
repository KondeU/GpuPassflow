#pragma once

#include <map>
#include "backend/BackendContext.h"

namespace au::gp {

struct InputProperties final {
    static rhi::InputVertexAttributes::Attribute MakeDefaultPositionVertexAttribute();
    std::vector<rhi::InputVertexAttributes::Attribute> vertexAttributes;
    rhi::InputIndexAttribute::Attribute indexAttribute;
};

struct OutputProperties final {
    enum class OutputSlot : int8_t {
        DS = -1,
        C0 = 0,
        C1 = 1,
        C2 = 2,
        C3 = 3,
        C4 = 4,
        C5 = 5,
        C6 = 6,
        C7 = 7
    };
    struct OutputAttribute final {
        rhi::BasicFormat imagePixelFormat;
        rhi::PassAction beginAction;
        rhi::PassAction endAction;
        rhi::ResourceState beforeState;
        rhi::ResourceState currentState;
        rhi::ResourceState afterState;
    };
    std::map<OutputSlot, OutputAttribute> targets;
};

struct ProgramProperties final {
    struct ShaderProgram final {
        std::string source;
        std::string entry;
    };
    std::map<rhi::ShaderStage, ShaderProgram> shaders;
};

struct ShaderResourceProperties final {
    static constexpr auto ResourceSpaceCount = 4;
    enum class ResourceSpace : uint8_t {
        PerObject = 0,
        PerView = 1,
        PerScene = 2,
        PerPass = 3
    };
    struct ResourceAttribute final {
        unsigned int baseBindingPoint;
        unsigned int bindingPointCount;
        rhi::ShaderStage resourceVisibility;
        rhi::DescriptorType resourceType;
        rhi::ResourceState beforeState;
        rhi::ResourceState currentState;
        rhi::ResourceState afterState;
    };
    std::map<ResourceSpace, std::vector<ResourceAttribute>> resources;
};

}
