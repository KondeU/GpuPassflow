#include "Resources.h"

namespace au::gp {

inline void SafeCopyMemory(void* dstAddr, size_t dstSize, const void* srcAddr, size_t srcSize)
{
    #ifdef _MSC_VER
    memcpy_s(dstAddr, dstSize, srcAddr, srcSize);
    #else
    memcpy(dstAddr, srcAddr, std::min(dstSize, srcSize));
    #endif
}

//////////////////////////////////////////////////
// ConstantBuffer<T>

template <typename T>
inline void ConstantBuffer<T>::ConfigureConstantBufferHeapType(rhi::TransferDirection type)
{
    description.memoryType = type;
}

template <typename T>
inline void ConstantBuffer<T>::SetupConstantBuffer()
{
    description.bufferBytesSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline T& ConstantBuffer<T>::AcquireConstantBuffer(bool update)
{
    if (update) {
        dirty.set();
    }
    if (!constantBufferData) {
        constantBufferData = std::make_unique<T>();
    }
    return *constantBufferData;
}

template <typename T>
inline void ConstantBuffer<T>::UpdateConstantBuffer(const T& value)
{
    SafeCopyMemory(&AcquireConstantBuffer(true), sizeof(T), &value, sizeof(value));
}

template <typename T>
inline void ConstantBuffer<T>::ReleaseConstantBuffer()
{
    constantBufferData.reset();
}

template <typename T>
inline void* ConstantBuffer<T>::RawCpuPtr()
{
    return &AcquireConstantBuffer(false);
}

//////////////////////////////////////////////////
// StructuredBuffer<T>

template <typename T>
inline void StructuredBuffer<T>::ConfigureStructuredBufferHeapType(rhi::TransferDirection type)
{
    description.memoryType = type;
}

template <typename T>
inline void StructuredBuffer<T>::ConfigureStructuredBufferWritable(bool writable)
{
    description.writableResourceInShader = writable;
}

template <typename T>
inline void StructuredBuffer<T>::SetupStructuredBuffer(unsigned int elementsCount)
{
    CheckSize(elementsCount);
    description.elementsCount = elementsCount;
    description.elementBytesSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline void StructuredBuffer<T>::ResizeStructuredBuffer(unsigned int elementsCount)
{
    CloseGPU();
    ReleaseStructuredBuffer();
    SetupStructuredBuffer(elementsCount);
}

template <typename T>
inline std::vector<T>& StructuredBuffer<T>::AcquireStructuredBuffer(bool update)
{
    if (update) {
        dirty.set();
    }
    if (structuredBufferData.empty()) {
        structuredBufferData.resize(description.elementsCount);
    }
    return structuredBufferData;
}

template <typename T>
inline void StructuredBuffer<T>::UpdateStructuredBuffer(
    const std::vector<T>& value, unsigned int offset)
{
    auto& buffer = AcquireStructuredBuffer(true);
    if (offset < buffer.size()) {
        SafeCopyMemory(
            buffer.data() + offset,
            (buffer.size() - offset) * sizeof(T),
            value.data(),
            value.size() * sizeof(T));
    }
}

template <typename T>
inline void StructuredBuffer<T>::ReleaseStructuredBuffer()
{
    structuredBufferData.resize(0);
}

template <typename T>
inline void* StructuredBuffer<T>::RawCpuPtr()
{
    return AcquireStructuredBuffer(false).data();
}

//////////////////////////////////////////////////
// IndexBuffer<T>

template <typename T>
inline void IndexBuffer<T>::SetupIndexBuffer(unsigned int indicesCount)
{
    CheckSize(indicesCount);
    description.indicesCount = indicesCount;
    description.indexByteSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline void IndexBuffer<T>::ResizeIndexBuffer(unsigned int indicesCount)
{
    CloseGPU();
    ReleaseIndexBuffer();
    SetupIndexBuffer(indicesCount);
}

template <typename T>
inline std::vector<T>& IndexBuffer<T>::AcquireIndexBuffer(bool update)
{
    if (update) {
        dirty.set();
    }
    if (indexBufferData.empty()) {
        indexBufferData.resize(description.indicesCount);
    }
    return indexBufferData;
}

template <typename T>
void IndexBuffer<T>::UpdateIndexBuffer(const std::vector<T>& value, unsigned int offset)
{
    auto& buffer = AcquireIndexBuffer(true);
    if (offset < buffer.size()) {
        SafeCopyMemory(
            buffer.data() + offset,
            (buffer.size() - offset) * sizeof(T),
            value.data(),
            value.size() * sizeof(T));
    }
}

template <typename T>
inline void IndexBuffer<T>::ReleaseIndexBuffer()
{
    indexBufferData.resize(0);
}

template <typename T>
inline void* IndexBuffer<T>::RawCpuPtr()
{
    return AcquireIndexBuffer(false).data();
}

//////////////////////////////////////////////////
// VertexBuffer<T>

template <typename T>
inline void VertexBuffer<T>::SetupVertexBuffer(unsigned int verticesCount)
{
    CheckSize(verticesCount);
    description.verticesCount = verticesCount;
    description.attributesByteSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline void VertexBuffer<T>::ResizeVertexBuffer(unsigned int verticesCount)
{
    CloseGPU();
    ReleaseVertexBuffer();
    SetupVertexBuffer(verticesCount);
}

template <typename T>
inline std::vector<T>& VertexBuffer<T>::AcquireVertexBuffer(bool update)
{
    if (update) {
        dirty.set();
    }
    if (vertexBufferData.empty()) {
        vertexBufferData.resize(description.verticesCount);
    }
    return vertexBufferData;
}

template <typename T>
inline void VertexBuffer<T>::UpdateVertexBuffer(const std::vector<T>& value, unsigned int offset)
{
    auto& buffer = AcquireVertexBuffer(true);
    if (offset < buffer.size()) {
        SafeCopyMemory(
            buffer.data() + offset,
            (buffer.size() - offset) * sizeof(T),
            value.data(),
            value.size() * sizeof(T));
    }
}

template <typename T>
inline void VertexBuffer<T>::ReleaseVertexBuffer()
{
    vertexBufferData.resize(0);
}

template <typename T>
inline void* VertexBuffer<T>::RawCpuPtr()
{
    return AcquireVertexBuffer(false).data();
}

//////////////////////////////////////////////////
// Texture<D>

template <unsigned int D>
inline void Texture<D>::ConfigureTextureUsage(rhi::ImageType usage)
{
    description.usage = usage;
}

template <unsigned int D>
inline void Texture<D>::ConfigureTextureHeapType(rhi::TransferDirection type)
{
    description.memoryType = type;
}

template <unsigned int D>
inline void Texture<D>::ConfigureTextureWritable(bool writable)
{
    description.writableResourceInShader = writable;
}

template <unsigned int D>
inline void Texture<D>::SetupTexture(rhi::BasicFormat format,
    unsigned int width, unsigned int height, unsigned int arrays)
{
    CheckSize(width, std::numeric_limits<decltype(description.width)>::max());
    CheckSize(height, std::numeric_limits<decltype(description.height)>::max());
    CheckSize(arrays, std::numeric_limits<decltype(description.arrays)>::max());

    if constexpr (D >= 1) {
        elementArray[0] = width;
    }
    if constexpr (D >= 2) {
        elementArray[1] = height;
    }
    if constexpr (D >= 3) {
        elementArray[2] = arrays;
    }
    elementSize = QueryBasicFormatBytes(format);

    description.format = format;
    description.width  = elementArray[0];
    description.height = elementArray[1];
    description.arrays = elementArray[2];
    description.dimension = static_cast<rhi::ImageDimension>(D);
    SetupGPU();
}

template <unsigned int D>
inline void Texture<D>::ResizeTexture(
    unsigned int width, unsigned int height, unsigned int arrays)
{
    CloseGPU();
    SetupTexture(description.format, width, height, arrays);
}

template <unsigned int D>
inline std::vector<uint8_t>& Texture<D>::AcquireTextureBuffer(bool update)
{
    if (update) {
        dirty.set();
    }
    if (pixelsBufferBytesData.empty()) {
        pixelsBufferBytesData.resize(static_cast<size_t>(elementSize)
            * elementArray[0] * elementArray[1] * elementArray[2]);
    }
    return pixelsBufferBytesData;
}

template <unsigned int D>
inline void Texture<D>::ReleaseTextureBuffer()
{
    pixelsBufferBytesData.resize(0);
}

template <unsigned int D>
inline unsigned int Texture<D>::GetDimensions() const
{
    return D;
}

template <unsigned int D>
inline void* Texture<D>::RawCpuPtr()
{
    return AcquireTextureBuffer(false).data();
}

}
