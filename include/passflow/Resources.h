#pragma once

#include "backend/BackendContext.h"

namespace au::gp {

template <typename T>
using Resource = std::shared_ptr<T>;

// Please use Passflow::MakeResource to create resource,
// otherwise device in the DeviceHolder will be nullptr!
class DeviceHolder {
public:
    void ConfigureAvoidInfight(bool infight);

protected:
    DeviceHolder() = default;
    virtual ~DeviceHolder() = 0;

    bool avoidInfight = true;

    rhi::Device* device = nullptr; // Not owned!
    unsigned int multipleBufferingCount = 0;

private:
    friend class Passflow;
    DeviceHolder(const DeviceHolder&) = delete;
    DeviceHolder& operator=(const DeviceHolder&) = delete;
};

//////////////////////////////////////////////////

class BaseConstantBuffer : public DeviceHolder {
public:
    BaseConstantBuffer() = default;
    ~BaseConstantBuffer() override;

    void UploadConstantBuffer(unsigned int index);
    void UploadConstantBuffers();

    virtual void* RawCpuPtr() = 0;
    rhi::ResourceBuffer* RawGpuInst(unsigned int index);

    Resource<BaseConstantBuffer> Clone() const;

protected:
    void SetupGPU();
    void CloseGPU();

    rhi::ResourceBuffer::Description description{ 0 }; // Default memory type: CPU_TO_GPU
    std::vector<rhi::ResourceBuffer*> buffers;
};

class BaseStructuredBuffer : public DeviceHolder {
public:
    BaseStructuredBuffer() = default;
    ~BaseStructuredBuffer() override;

    void UploadStructuredBuffer(unsigned int index);
    void UploadStructuredBuffers();

    virtual void* RawCpuPtr() = 0;
    rhi::ResourceBufferEx* RawGpuInst(unsigned int index);

    Resource<BaseStructuredBuffer> Clone() const;

protected:
    void SetupGPU();
    void CloseGPU();

    rhi::ResourceBufferEx::Description description{ 0, 0 }; // Default memory type: GPU_ONLY
    std::vector<rhi::ResourceBufferEx*> buffers;
};

class BaseIndexBuffer : public DeviceHolder {
public:
    BaseIndexBuffer() = default;
    ~BaseIndexBuffer() override;

    void UploadIndexBuffer(unsigned int index);
    void UploadIndexBuffers();

    virtual void* RawCpuPtr() = 0;
    rhi::InputIndex* RawGpuInst(unsigned int index);

    Resource<BaseIndexBuffer> Clone() const;

protected:
    void SetupGPU();
    void CloseGPU();

    rhi::InputIndex::Description description{ 0, 0 }; // Default memory type: GPU_ONLY
    std::vector<rhi::InputIndex*> indices;
};

class BaseVertexBuffer : public DeviceHolder {
public:
    BaseVertexBuffer() = default;
    ~BaseVertexBuffer() override;

    void UploadVertexBuffer(unsigned int index);
    void UploadVertexBuffers();

    virtual void* RawCpuPtr() = 0;
    rhi::InputVertex* RawGpuInst(unsigned int index);

    Resource<BaseVertexBuffer> Clone() const;

protected:
    void SetupGPU();
    void CloseGPU();

    rhi::InputVertex::Description description{ 0, 0 }; // Default memory type: GPU_ONLY
    std::vector<rhi::InputVertex*> vertices;
};

class BaseTexture : public DeviceHolder {
public:
    BaseTexture() = default;
    ~BaseTexture() override;

    void UploadTextureBuffer(unsigned int index);
    void UploadTextureBuffers();

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    unsigned int GetArrays() const;
    void GetSize(unsigned int& width, unsigned int& height, unsigned int& arrays) const;
    virtual unsigned int GetDimensions() const = 0;

    virtual void* RawCpuPtr() = 0;
    rhi::ResourceImage* RawGpuInst(unsigned int index);

    Resource<BaseTexture> Clone() const;

protected:
    void SetupGPU();
    void CloseGPU();

    rhi::ResourceImage::Description description{ rhi::BasicFormat::R32G32B32A32_FLOAT, 1, 1 };
    std::vector<rhi::ResourceImage*> images; // Default memory type: GPU_ONLY
};

//////////////////////////////////////////////////

template <typename T>
class ConstantBuffer final : public BaseConstantBuffer {
public:
    static_assert(std::is_trivial<T>::value && std::is_standard_layout<T>::value,
        "The specialization type of ConstantBuffer must be POD!");

    void ConfigureConstantBufferHeapType(rhi::TransferDirection type);

    void SetupConstantBuffer();

    T& AcquireConstantBuffer();
    void UpdateConstantBuffer(const T& value);

    void ReleaseConstantBuffer(); // Free the host memory.

protected:
    void* RawCpuPtr() override;

private:
    std::unique_ptr<T> constantBufferData;
};

template <typename T>
class StructuredBuffer final : public BaseStructuredBuffer {
public:
    static_assert(std::is_trivial<T>::value && std::is_standard_layout<T>::value,
        "The specialization type of StructuredBuffer must be POD!");

    void ConfigureStructuredBufferHeapType(rhi::TransferDirection type);
    void ConfigureStructuredBufferWritable(bool writable);

    void SetupStructuredBuffer(unsigned int elementsCount);
    void ResizeStructuredBuffer(unsigned int elementsCount);

    std::vector<T>& AcquireStructuredBuffer();
    void UpdateStructuredBuffer(const std::vector<T>& value, unsigned int offset);

    void ReleaseStructuredBuffer(); // Free the host memory.

protected:
    void* RawCpuPtr() override;

private:
    std::vector<T> structuredBufferData;
};

template <typename T>
class IndexBuffer final : public BaseIndexBuffer {
public:
    static_assert(std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value,
        "The specialization type of IndexBuffer must be uint16_t or uint32_t!");

    void SetupIndexBuffer(unsigned int indicesCount);
    void ResizeIndexBuffer(unsigned int indicesCount);

    std::vector<T>& AcquireIndexBuffer();
    void UpdateIndexBuffer(const std::vector<T>& value, unsigned int offset);

    void ReleaseIndexBuffer(); // Free the host memory.

protected:
    void* RawCpuPtr() override;

private:
    std::vector<T> indexBufferData;
};

template <typename T>
class VertexBuffer final : public BaseVertexBuffer {
public:
    static_assert(std::is_trivial<T>::value && std::is_standard_layout<T>::value,
        "The specialization type of VertexBuffer must be POD!");

    void SetupVertexBuffer(unsigned int verticesCount);
    void ResizeVertexBuffer(unsigned int verticesCount);

    std::vector<T>& AcquireVertexBuffer();
    void UpdateVertexBuffer(const std::vector<T>& value, unsigned int offset);

    void ReleaseVertexBuffer(); // Free the host memory.

protected:
    void* RawCpuPtr() override;

private:
    std::vector<T> vertexBufferData;
};

template <unsigned int D>
class Texture : public BaseTexture {
public:
    static_assert((D == 1) || (D == 2) || (D == 3),
        "The specialization dimension of Texture must be 1 or 2 or 3!");

    void ConfigureTextureUsage(rhi::ImageType usage);
    void ConfigureTextureHeapType(rhi::TransferDirection type);
    void ConfigureTextureWritable(bool writable);

    void SetupTexture(rhi::BasicFormat format,
        uint32_t width, uint32_t height = 1, uint8_t arrays = 1);
    void ResizeTexture(uint32_t width, uint32_t height = 1, uint8_t arrays = 1);

    std::vector<uint8_t>& AcquireTextureBuffer();

    void ReleaseTextureBuffer(); // Free the host memory.

    unsigned int GetDimensions() const override;

protected:
    void* RawCpuPtr() override;

private:
    unsigned int elementSize = 0;
    unsigned int elementArray[3] = { 1, 1, 1 };
    std::vector<uint8_t> pixelsBufferBytesData;
};
using Texture1D = Texture<1>;
using Texture2D = Texture<2>;
using Texture3D = Texture<3>;

//////////////////////////////////////////////////

class ColorOutput final : public BaseTexture {
public:
    void SetupColorOutput(rhi::BasicFormat format, unsigned int width, unsigned int height);
    void ResizeColorOutput(unsigned int width, unsigned int height);

    std::vector<uint8_t>& AcquireColorBuffer();

    void ReleaseColorBuffer(); // Free the host memory.

    unsigned int GetDimensions() const override;

protected:
    void* RawCpuPtr() override;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int pixelBytes = 0; // Bytes of a pixel.
    std::vector<uint8_t> pixelsBufferBytesData;
};

class DepthStencilOutput final : public BaseTexture {
public:
    void SetupDepthStencilOutput(rhi::BasicFormat format, unsigned int width, unsigned int height);
    void ResizeDepthStencilOutput(unsigned int width, unsigned int height);

    std::vector<uint8_t>& AcquireDepthStencilBuffer();

    void ReleaseDepthStencilBuffer(); // Free the host memory.

    unsigned int GetDimensions() const override;

protected:
    void* RawCpuPtr() override;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int pixelBytes = 0; // Bytes of a pixel.
    std::vector<uint8_t> pixelsBufferBytesData;
};

class DisplayPresentOutput final : public DeviceHolder {
public:
    DisplayPresentOutput();
    ~DisplayPresentOutput();

    void SetupDisplayPresentOutput(rhi::BasicFormat format,
        unsigned int width, unsigned int height, void* window);
    void ResizeDisplay(unsigned int width, unsigned int height);

    void* GetWindow() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    void GetSize(unsigned int& width, unsigned int& height) const;

    rhi::Swapchain* RawGpuInst();

protected:
    rhi::Swapchain::Description description{ nullptr, 0, 0 };
    rhi::Swapchain* swapchain = nullptr;
};

class Sampler : public DeviceHolder {
public:
    Sampler() = default;
    ~Sampler() override;

    void ConfigureSamplerState(rhi::SamplerState::Filter filterForAll);
    void ConfigureSamplerState(rhi::AddressMode addressModeForUVW);
    void ConfigureSamplerState(rhi::SamplerState samplerState);
    void SetupSampler();

    rhi::ImageSampler* RawGpuInst();

    Resource<Sampler> Clone() const;

private:
    rhi::ImageSampler::Description description{
        rhi::SamplerState::Filter::Linear, rhi::AddressMode::Wrap };
    rhi::ImageSampler* sampler = nullptr;
};

}

#include "Resources.inl"
