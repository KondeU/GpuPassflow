#pragma once

#include <map>
#include "backend/BackendContext.h"

namespace au::gp {

class Passflow;

class BasePass {
public:
    virtual ~BasePass() = default;

    virtual void OnPreparePass(rhi::Device* device) = 0;
    virtual void OnBeforePass(unsigned int currentBufferingIndex) = 0;
    virtual void OnExecutePass(rhi::CommandRecorder* recorder) = 0;
    virtual void OnAfterPass(unsigned int currentPassInFlowIndex) = 0;
    virtual void OnEnablePass(bool enable) = 0;

protected:
    explicit BasePass(Passflow& passflow);

    struct InputProperties final {
        static rhi::InputVertexAttributes::Attribute MakeDefaultPositionVertexAttribute();
        std::vector<rhi::InputVertexAttributes::Attribute> vertexAttributes;
        rhi::InputIndexAttribute::Attribute indexAttribute;
        bool multipleObjects = true;
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
        enum class ResourceSpace : uint8_t {
            PerObject = 0,
            PerView   = 1,
            PerScene  = 2,
            PerPass   = 3
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

    class DynamicDescriptorManager final {
    public:
        DynamicDescriptorManager(rhi::Device* device, rhi::DescriptorType type);
        ~DynamicDescriptorManager();

        void ReallocateDescriptorHeap(unsigned int descriptorCount);
        rhi::Descriptor* AcquireDescriptor(unsigned int index);
        rhi::DescriptorHeap* AcquireDescriptorHeap();

    private:
        GP_LOG_TAG(DynamicDescriptorManager);

        void FreeDescriptorHeap();

        rhi::DescriptorType heapType;
        rhi::Device* device; // Not owned!
        rhi::DescriptorHeap* descriptorHeap = nullptr;
        std::vector<rhi::Descriptor*> descriptors;
    };

    Passflow& passflow;

    const unsigned int multipleBufferingCount;

private:
    GP_LOG_TAG(BasePass);
};

}
