#pragma once

namespace au::rhi {

// Push constants object in Vulkan
// Root constants object in DX12
class ConstantBuffer {
public:
    struct Description final {
        unsigned int bufferBytesSize = 128; // Min size that is compatible with Vulkan and DX12.
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    ConstantBuffer() = default;
    virtual ~ConstantBuffer() = default;
};

}
