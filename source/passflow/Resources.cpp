#include "passflow/Resources.h"

namespace {

GP_LOG_TAG(Resources);

template <typename Resource>
void UploadHost(Resource* destination, const void* source, size_t size, size_t element = 1)
{
    auto totalBytes = size * element;
    au::gp::SafeCopyMemory(destination->Map(), totalBytes, source, totalBytes);
    destination->Unmap();
}

template <typename Resource>
void UploadRemote(au::rhi::Device* device,
    Resource* destination, Resource* staging, const void* source, size_t size, size_t element = 1)
{
    auto command = device->CreateCommandRecorder({ "Upload", au::rhi::CommandType::Transfer });

    command->BeginRecord();
    command->RcBarrier(destination,
        au::rhi::ResourceState::GENERAL_READ,
        au::rhi::ResourceState::COPY_DESTINATION);

    auto totalBytes = size * element;
    command->RcUpload(source, totalBytes, destination, staging);

    command->RcBarrier(destination,
        au::rhi::ResourceState::COPY_DESTINATION,
        au::rhi::ResourceState::GENERAL_READ);
    command->EndRecord();

    command->Submit();
    command->Wait();

    device->ReleaseCommandRecordersMemory("Upload");
    device->DestroyCommandRecorder(command);
}

}

namespace au::gp {

DeviceHolder::~DeviceHolder()
{
    // Pure virtual destruct function need to provide the implementation of the function,
    // while general pure virtual functions cannot have implementations, because that
    // pure virtual destructor need to be called to destruct the base class object,
    // although this is a abstract class. If the implementation of the destructor is not
    // provided, it will cause the problem when process destruction.
}

void DeviceHolder::ConfigureAvoidInfight(bool infight)
{
    avoidInfight = infight;
}

//////////////////////////////////////////////////
// BaseConstantBuffer

BaseConstantBuffer::~BaseConstantBuffer()
{
    CloseGPU();
}

void BaseConstantBuffer::UploadConstantBuffer(unsigned int index)
{
    if (index >= buffers.size()) {
        GP_LOG_RET_W(TAG, "Upload constant buffer failed, index out of range.");
    }
    auto buffer = buffers[index];
    if (description.memoryType == rhi::TransferDirection::CPU_TO_GPU) {
        UploadHost(buffer, RawCpuPtr(), description.bufferBytesSize);
    } else if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        auto staging = device->CreateResourceBuffer({ description.bufferBytesSize });
        UploadRemote(device, buffer, staging, RawCpuPtr(), description.bufferBytesSize);
        device->DestroyResourceBuffer(staging);
    } else {
        GP_LOG_W(TAG, "Upload constant buffer failed, this buffer is in the readback heap.");
    }
}

void BaseConstantBuffer::UploadConstantBuffers()
{
    for (unsigned int index = 0; index < buffers.size(); index++) {
        UploadConstantBuffer(index);
    }
}

rhi::ResourceBuffer* BaseConstantBuffer::RawGpuInst(unsigned int index)
{
    if (index >= buffers.size()) {
        GP_LOG_RETN_W(TAG, "Acquire constant buffer backend instance failed, index out of range.");
    }
    return buffers[index];
}

Resource<BaseConstantBuffer> BaseConstantBuffer::Clone() const
{
    // TODO
    return Resource<BaseConstantBuffer>();
}

void BaseConstantBuffer::SetupGPU()
{
    if (!buffers.empty()) {
        GP_LOG_RET_W(TAG, "The constant buffer GPU resource `%p` has already been setup.", this);
    }
    buffers.resize(avoidInfight ? multipleBufferingCount : 1);
    for (auto& buffer : buffers) {
        buffer = device->CreateResourceBuffer(description);
    }
}

void BaseConstantBuffer::CloseGPU()
{
    for (auto buffer : buffers) {
        device->DestroyResourceBuffer(buffer);
    }
    buffers.resize(0);
}

//////////////////////////////////////////////////
// BaseStructuredBuffer

BaseStructuredBuffer::~BaseStructuredBuffer()
{
    CloseGPU();
}

void BaseStructuredBuffer::UploadStructuredBuffer(unsigned int index)
{
    if (index >= buffers.size()) {
        GP_LOG_RET_W(TAG, "Upload structured buffer failed, index out of range.");
    }
    auto buffer = buffers[index];
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        auto staging = device->CreateResourceBuffer({ description.elementsCount,
            description.elementBytesSize, false, rhi::TransferDirection::CPU_TO_GPU });
        UploadRemote(device, buffer, staging, RawCpuPtr(),
            description.elementBytesSize, description.elementsCount);
        device->DestroyResourceBuffer(staging);
    } else if (description.memoryType == rhi::TransferDirection::CPU_TO_GPU) {
        UploadHost(buffer, RawCpuPtr(), description.elementBytesSize, description.elementsCount);
    } else {
        GP_LOG_W(TAG, "Upload structured buffer failed, this buffer is in the readback heap.");
    }
}

void BaseStructuredBuffer::UploadStructuredBuffers()
{
    for (unsigned int index = 0; index < buffers.size(); index++) {
        UploadStructuredBuffer(index);
    }
}

rhi::ResourceBufferEx* BaseStructuredBuffer::RawGpuInst(unsigned int index)
{
    if (index >= buffers.size()) {
        GP_LOG_RETN_W(TAG,
            "Acquire structured buffer backend instance failed, index out of range.");
    }
    return buffers[index];
}

void BaseStructuredBuffer::SetupGPU()
{
    if (!buffers.empty()) {
        GP_LOG_RET_W(TAG, "The structured buffer GPU resource `%p` has already been setup.", this);
    }
    buffers.resize(avoidInfight ? multipleBufferingCount : 1);
    for (auto& buffer : buffers) {
        buffer = device->CreateResourceBuffer(description);
    }
}

void BaseStructuredBuffer::CloseGPU()
{
    for (auto buffer : buffers) {
        device->DestroyResourceBuffer(buffer);
    }
    buffers.resize(0);
}

Resource<BaseStructuredBuffer> BaseStructuredBuffer::Clone() const
{
    // TODO
    return Resource<BaseStructuredBuffer>();
}

//////////////////////////////////////////////////
// BaseIndexBuffer

BaseIndexBuffer::~BaseIndexBuffer()
{
    CloseGPU();
}

void BaseIndexBuffer::UploadIndexBuffer(unsigned int index)
{
    if (index >= indices.size()) {
        GP_LOG_RET_W(TAG, "Upload index buffer failed, index out of range.");
    }
    auto indexBuffer = indices[index];
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        auto staging = device->CreateInputIndex({ description.indicesCount,
            description.indexByteSize, rhi::TransferDirection::CPU_TO_GPU });
        UploadRemote(device, indexBuffer, staging, RawCpuPtr(),
            description.indexByteSize, description.indicesCount);
        device->DestroyInputIndex(staging);
    } else if (description.memoryType == rhi::TransferDirection::CPU_TO_GPU) {
        UploadHost(indexBuffer, RawCpuPtr(), description.indexByteSize, description.indicesCount);
    } else {
        GP_LOG_W(TAG, "Upload index buffer failed, this buffer is in the readback heap.");
    }
}

void BaseIndexBuffer::UploadIndexBuffers()
{
    for (unsigned int index = 0; index < indices.size(); index++) {
        UploadIndexBuffer(index);
    }
}

rhi::InputIndex* BaseIndexBuffer::RawGpuInst(unsigned int index)
{
    if (index >= indices.size()) {
        GP_LOG_RETN_W(TAG, "Acquire index buffer backend instance failed, index out of range.");
    }
    return indices[index];
}

void BaseIndexBuffer::SetupGPU()
{
    if (!indices.empty()) {
        GP_LOG_RET_W(TAG, "The index buffer GPU resource `%p` has already been setup.", this);
    }
    indices.resize(avoidInfight ? multipleBufferingCount : 1);
    for (auto& index : indices) {
        index = device->CreateInputIndex(description);
    }
}

void BaseIndexBuffer::CloseGPU()
{
    for (auto index : indices) {
        device->DestroyInputIndex(index);
    }
    indices.resize(0);
}

Resource<BaseIndexBuffer> BaseIndexBuffer::Clone() const
{
    // TODO
    return Resource<BaseIndexBuffer>();
}

//////////////////////////////////////////////////
// BaseVertexBuffer

BaseVertexBuffer::~BaseVertexBuffer()
{
    CloseGPU();
}

void BaseVertexBuffer::UploadVertexBuffer(unsigned int index)
{
    if (index >= vertices.size()) {
        GP_LOG_RET_W(TAG, "Upload vertex buffer failed, index out of range.");
    }
    auto vertexBuffer = vertices[index];
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        auto staging = device->CreateInputVertex({ description.verticesCount,
            description.attributesByteSize, rhi::TransferDirection::CPU_TO_GPU });
        UploadRemote(device, vertexBuffer, staging, RawCpuPtr(),
            description.attributesByteSize, description.verticesCount);
        device->DestroyInputVertex(staging);
    } else if (description.memoryType == rhi::TransferDirection::CPU_TO_GPU) {
        UploadHost(vertexBuffer, RawCpuPtr(),
            description.attributesByteSize, description.verticesCount);
    } else {
        GP_LOG_W(TAG, "Upload vertex buffer failed, this buffer is in the readback heap.");
    }
}

void BaseVertexBuffer::UploadVertexBuffers()
{
    for (unsigned int index = 0; index < vertices.size(); index++) {
        UploadVertexBuffer(index);
    }
}

rhi::InputVertex* BaseVertexBuffer::RawGpuInst(unsigned int index)
{
    if (index >= vertices.size()) {
        GP_LOG_RETN_W(TAG, "Acquire vertex buffer backend instance failed, index out of range.");
    }
    return vertices[index];
}

void BaseVertexBuffer::SetupGPU()
{
    if (!vertices.empty()) {
        GP_LOG_RET_W(TAG, "The vertex buffer GPU resource `%p` has already been setup.", this);
    }
    vertices.resize(avoidInfight ? multipleBufferingCount : 1);
    for (auto& vertex : vertices) {
        vertex = device->CreateInputVertex(description);
    }
}

void BaseVertexBuffer::CloseGPU()
{
    for (auto vertex : vertices) {
        device->DestroyInputVertex(vertex);
    }
    vertices.resize(0);
}

Resource<BaseVertexBuffer> BaseVertexBuffer::Clone() const
{
    // TODO
    return Resource<BaseVertexBuffer>();
}

//////////////////////////////////////////////////
// BaseTexture

BaseTexture::~BaseTexture()
{
    CloseGPU();
}

void BaseTexture::UploadTextureBuffer(unsigned int index)
{
    if (index >= images.size()) {
        GP_LOG_RET_W(TAG, "Upload texture buffer failed, index out of range.");
    }
    auto image = images[index];
    // TODO: Current only support 1x MSAA and 1 mipmap.
    size_t bytes = static_cast<size_t>(QueryBasicFormatBytes(description.format))
        * description.width * description.height * description.arrays;
    if (description.memoryType == rhi::TransferDirection::GPU_ONLY) {
        auto stagingImageDescription = description;
        stagingImageDescription.usage = rhi::ImageType::ShaderResource;
        stagingImageDescription.memoryType = rhi::TransferDirection::CPU_TO_GPU;
        auto staging = device->CreateResourceImage(stagingImageDescription);
        UploadRemote(device, image, staging, RawCpuPtr(), bytes);
        device->DestroyResourceImage(staging);
    } else if (description.memoryType == rhi::TransferDirection::CPU_TO_GPU) {
        UploadHost(image, RawCpuPtr(), bytes);
    } else {
        GP_LOG_W(TAG, "Upload texture buffer failed, this buffer is in the readback heap.");
    }
}

void BaseTexture::UploadTextureBuffers()
{
    for (unsigned int index = 0; index < images.size(); index++) {
        UploadTextureBuffer(index);
    }
}

unsigned int BaseTexture::GetWidth() const
{
    return description.width;
}

unsigned int BaseTexture::GetHeight() const
{
    return description.height;
}

unsigned int BaseTexture::GetArrays() const
{
    return description.arrays;
}

void BaseTexture::GetSize(unsigned int& width, unsigned int& height, unsigned int& arrays) const
{
    width = description.width;
    height = description.height;
    arrays = description.arrays;
}

rhi::ResourceImage* BaseTexture::RawGpuInst(unsigned int index)
{
    if (index >= images.size()) {
        GP_LOG_RETN_W(TAG, "Acquire texture backend instance failed, index out of range.");
    }
    return images[index];
}

void BaseTexture::SetupGPU()
{
    if (!images.empty()) {
        GP_LOG_RET_W(TAG, "The texture GPU resource `%p` has already been setup.", this);
    }
    images.resize(avoidInfight ? multipleBufferingCount : 1);
    for (auto& image : images) {
        image = device->CreateResourceImage(description);
    }
}

void BaseTexture::CloseGPU()
{
    for (auto image : images) {
        device->DestroyResourceImage(image);
    }
    images.resize(0);
}

Resource<BaseTexture> BaseTexture::Clone() const
{
    // TODO
    return Resource<BaseTexture>();
}

//////////////////////////////////////////////////

void ColorOutput::SetupColorOutput(
    rhi::BasicFormat format, unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    pixelBytes = QueryBasicFormatBytes(format);

    description.width = width;
    description.height = height;
    description.arrays = 1;
    description.format = format;
    description.usage = rhi::ImageType::Color;
    description.dimension = rhi::ImageDimension::Dimension2D;
    SetupGPU();
}

void ColorOutput::ResizeColorOutput(unsigned int width, unsigned int height)
{
    CloseGPU();
    ReleaseColorBuffer();
    SetupColorOutput(description.format, width, height);
}

std::vector<uint8_t>& ColorOutput::AcquireColorBuffer()
{
    if (pixelsBufferBytesData.empty()) {
        pixelsBufferBytesData.resize(static_cast<size_t>(pixelBytes) * width * height);
    }
    return pixelsBufferBytesData;
}

void ColorOutput::ReleaseColorBuffer()
{
    pixelsBufferBytesData.resize(0);
}

unsigned int ColorOutput::GetDimensions() const
{
    return 2u; // It has width and height dimensions.
}

void* ColorOutput::RawCpuPtr()
{
    return AcquireColorBuffer().data();
}

void DepthStencilOutput::SetupDepthStencilOutput(
    rhi::BasicFormat format, unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    pixelBytes = QueryBasicFormatBytes(format);

    std::underlying_type<rhi::ImageType>::type usage = 0;
    if (IsBasicFormatHasDepth(format)) {
        usage |= EnumCast(rhi::ImageType::Depth);
    }
    if (IsBasicFormatHasStencil(format)) {
        usage |= EnumCast(rhi::ImageType::Stencil);
    }

    description.width = width;
    description.height = height;
    description.arrays = 1;
    description.format = format;
    description.usage = static_cast<rhi::ImageType>(usage);
    description.dimension = rhi::ImageDimension::Dimension2D;
    SetupGPU();
}

void DepthStencilOutput::ResizeDepthStencilOutput(unsigned int width, unsigned int height)
{
    CloseGPU();
    ReleaseDepthStencilBuffer();
    SetupDepthStencilOutput(description.format, width, height);
}

std::vector<uint8_t>& DepthStencilOutput::AcquireDepthStencilBuffer()
{
    if (pixelsBufferBytesData.empty()) {
        pixelsBufferBytesData.resize(static_cast<size_t>(pixelBytes) * width * height);
    }
    return pixelsBufferBytesData;
}

void DepthStencilOutput::ReleaseDepthStencilBuffer()
{
    pixelsBufferBytesData.resize(0);
}

unsigned int DepthStencilOutput::GetDimensions() const
{
    return 2u; // It has width and height dimensions.
}

void* DepthStencilOutput::RawCpuPtr()
{
    return AcquireDepthStencilBuffer().data();
}

DisplayPresentOutput::DisplayPresentOutput()
{
    description.bufferCount = multipleBufferingCount;
}

DisplayPresentOutput::~DisplayPresentOutput()
{
    if (swapchain) {
        device->DestroySwapchain(swapchain);
    }
}

void DisplayPresentOutput::SetupDisplayPresentOutput(
    rhi::BasicFormat format, unsigned int width, unsigned int height, void* window)
{
    description.width = width;
    description.height = height;
    description.window = window;
    description.colorFormat = format;
    description.isEnabledDepthStencil = false;

    if (swapchain) {
        GP_LOG_RET_W(TAG, "The swapchain GPU resource `%p` has already been setup.", this);
    }
    swapchain = device->CreateSwapchain(description);
}

void DisplayPresentOutput::ResizeDisplay(unsigned int width, unsigned int height)
{
    description.width = width;
    description.height = height;

    if (swapchain) {
        swapchain->Resize(width, height); // Swapchain resize will call WaitIdle.
    } else {
        GP_LOG_RET_W(TAG, "Resize failed, DisplayPresentOutput `%p` has not been setup.", this);
    }
}

void* DisplayPresentOutput::GetWindow() const
{
    return description.window;
}

unsigned int DisplayPresentOutput::GetWidth() const
{
    return description.width;
}

unsigned int DisplayPresentOutput::GetHeight() const
{
    return description.height;
}

void DisplayPresentOutput::GetSize(unsigned int& width, unsigned int& height) const
{
    width = description.width;
    height = description.height;
}

rhi::Swapchain* DisplayPresentOutput::RawGpuInst()
{
    return swapchain;
}

Sampler::~Sampler()
{
    if (sampler) {
        device->DestroyImageSampler(sampler);
    }
}

void Sampler::ConfigureSamplerState(rhi::SamplerState::Filter filterForAll)
{
    description.state.minification  = filterForAll;
    description.state.magnification = filterForAll;
    description.state.mipLevel      = filterForAll;
}

void Sampler::ConfigureSamplerState(rhi::AddressMode addressModeForUVW)
{
    description.state.addressMode[0] = addressModeForUVW;
    description.state.addressMode[1] = addressModeForUVW;
    description.state.addressMode[2] = addressModeForUVW;
}

void Sampler::ConfigureSamplerState(rhi::SamplerState samplerState)
{
    description.state = samplerState;
}

void Sampler::SetupSampler()
{
    if (sampler) {
        GP_LOG_RET_W(TAG, "The image sampler GPU resource `%p` has already been setup.", this);
    }
    sampler = device->CreateImageSampler(description);
}

rhi::ImageSampler* Sampler::RawGpuInst()
{
    return sampler;
}

Resource<Sampler> Sampler::Clone() const
{
    // TODO
    return Resource<Sampler>();
}

}
