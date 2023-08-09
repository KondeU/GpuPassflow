#pragma once

#include <map>
#include "framework/dfx/Logger.hpp"
#include "backend/BackendContext.h"

namespace au::passflow {

class Passflow;

class BasePass {
public:
    virtual ~BasePass() = default;

    virtual void OnPreparePass(backend::Device* device) = 0;
    virtual void OnBeforePass(unsigned int currentBufferingIndex) = 0;
    virtual void OnExecutePass(backend::CommandRecorder* recorder) = 0;
    virtual void OnAfterPass(unsigned int currentPassInFlowIndex) = 0;
    virtual void OnEnablePass(bool enable) = 0;

protected:
    explicit BasePass(Passflow& passflow);

    struct InputProperties final {
        static backend::InputVertexAttributes::Attribute MakeDefaultPositionVertexAttribute();
        std::vector<backend::InputVertexAttributes::Attribute> vertexAttributes;
        backend::InputIndexAttribute::Attribute indexAttribute;
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
            BasicFormat imagePixelFormat;
            PassAction beginAction;
            PassAction endAction;
            ResourceState beforeState;
            ResourceState currentState;
            ResourceState afterState;
        };
        std::map<OutputSlot, OutputAttribute> targets;
    };

    struct ProgramProperties final {
        struct ShaderProgram final {
            std::string source;
            std::string entry;
        };
        std::map<ShaderStage, ShaderProgram> shaders;
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
            ShaderStage resourceVisibility;
            DescriptorType resourceType;
            ResourceState beforeState;
            ResourceState currentState;
            ResourceState afterState;
        };
        std::map<ResourceSpace, std::vector<ResourceAttribute>> resources;
    };

    class DynamicDescriptorManager final {
    public:
        DynamicDescriptorManager(backend::Device* device, DescriptorType type);
        ~DynamicDescriptorManager();

        void ReallocateDescriptorHeap(unsigned int descriptorCount);
        backend::Descriptor* AcquireDescriptor(unsigned int index);
        backend::DescriptorHeap* AcquireDescriptorHeap();

    private:
        GP_LOG_TAG(DynamicDescriptorManager);

        void FreeDescriptorHeap();

        DescriptorType heapType;
        backend::Device* device; // Not owned!
        backend::DescriptorHeap* descriptorHeap = nullptr;
        std::vector<backend::Descriptor*> descriptors;
    };

    Passflow& passflow;

    unsigned int multipleBufferingCount;

private:
    GP_LOG_TAG(BasePass);
};

}
