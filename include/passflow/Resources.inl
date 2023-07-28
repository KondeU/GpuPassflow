#include "Resources.h"

namespace au::passflow {

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
inline void ConstantBuffer<T>::ConfigureConstantBufferHeapType(TransferDirection type)
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
inline T& ConstantBuffer<T>::AcquireConstantBuffer()
{
    if (!constantBufferData) {
        constantBufferData = std::make_unique<T>();
    }
    return *constantBufferData;
}

template <typename T>
inline void ConstantBuffer<T>::UpdateConstantBuffer(const T& value)
{
    SafeCopyMemory(&AcquireConstantBuffer(), sizeof(T), &value, sizeof(value));
}

template <typename T>
inline void ConstantBuffer<T>::ReleaseConstantBuffer()
{
    constantBufferData.reset();
}

template <typename T>
inline void* ConstantBuffer<T>::RawCpuPtr()
{
    return &AcquireConstantBuffer();
}

//////////////////////////////////////////////////
// StructuredBuffer<T>

template <typename T>
inline void StructuredBuffer<T>::ConfigureStructuredBufferHeapType(TransferDirection type)
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
inline std::vector<T>& StructuredBuffer<T>::AcquireStructuredBuffer()
{
    if (structuredBufferData.empty()) {
        structuredBufferData.resize(description.elementsCount);
    }
    return structuredBufferData;
}

template <typename T>
inline void StructuredBuffer<T>::UpdateStructuredBuffer(
    const std::vector<T>& value, unsigned int offset)
{
    SafeCopyMemory(
        AcquireStructuredBuffer().data() + offset,
        (AcquireStructuredBuffer().size() - offset) * sizeof(T),
        value.data(),
        value.size() * sizeof(T));
}

template <typename T>
inline void StructuredBuffer<T>::ReleaseStructuredBuffer()
{
    structuredBufferData.resize(0);
}

template <typename T>
inline void* StructuredBuffer<T>::RawCpuPtr()
{
    return AcquireStructuredBuffer().data();
}

//////////////////////////////////////////////////
// IndexBuffer<T>

template <typename T>
inline void IndexBuffer<T>::SetupIndexBuffer(unsigned int indicesCount)
{
    description.indicesCount = indicesCount;
    description.indexByteSize = sizeof(T);
    SetupGPU();
}

template <typename T>
inline void IndexBuffer<T>::ResizeIndexBuffer(unsigned int indicesCount)
{
    CloseGPU();
    ReleaseIndexBuffer();
    SetupStructuredBuffer(elementsCount);
}

template <typename T>
inline std::vector<T>& IndexBuffer<T>::AcquireIndexBuffer()
{
    if (indexBufferData.empty()) {
        indexBufferData.resize(description.indicesCount);
    }
    return indexBufferData;
}

template <typename T>
void IndexBuffer<T>::UpdateIndexBuffer(const std::vector<T>& value, unsigned int offset)
{
    SafeCopyMemory(
        AcquireIndexBuffer().data() + offset,
        (AcquireIndexBuffer().size() - offset) * sizeof(T),
        value.data(),
        value.size() * sizeof(T));
}

template <typename T>
inline void IndexBuffer<T>::ReleaseIndexBuffer()
{
    indexBufferData.resize(0);
}

template <typename T>
inline void* IndexBuffer<T>::RawCpuPtr()
{
    return AcquireIndexBuffer().data();
}

//////////////////////////////////////////////////
// VertexBuffer<T>

template <typename T>
inline void VertexBuffer<T>::SetupVertexBuffer(unsigned int verticesCount)
{
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
inline std::vector<T>& VertexBuffer<T>::AcquireVertexBuffer()
{
    if (vertexBufferData.empty()) {
        vertexBufferData.resize(description.verticesCount);
    }
    return vertexBufferData;
}

template <typename T>
inline void VertexBuffer<T>::UpdateVertexBuffer(const std::vector<T>& value, unsigned int offset)
{
    SafeCopyMemory(
        AcquireVertexBuffer().data() + offset,
        (AcquireVertexBuffer().size() - offset) * sizeof(T),
        value.data(),
        value.size() * sizeof(T));
}

template <typename T>
inline void VertexBuffer<T>::ReleaseVertexBuffer()
{
    vertexBufferData.resize(0);
}

template <typename T>
inline void* VertexBuffer<T>::RawCpuPtr()
{
    return AcquireVertexBuffer().data();
}

//////////////////////////////////////////////////
// Texture<D>

template <unsigned int D>
inline void Texture<D>::ConfigureTextureUsage(ImageType usage)
{
    description.usage = usage;
}

template <unsigned int D>
inline void Texture<D>::ConfigureTextureHeapType(TransferDirection type)
{
    description.memoryType = type;
}

template <unsigned int D>
inline void Texture<D>::ConfigureTextureWritable(bool writable)
{
    description.writableResourceInShader = writable;
}

template <unsigned int D>
inline void Texture<D>::SetupTexture(BasicFormat format,
    uint32_t width, uint32_t height, uint8_t arrays)
{
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
    description.dimension = static_cast<ImageDimension>(D);
    SetupGPU();
}

template <unsigned int D>
inline void Texture<D>::ResizeTexture(
    uint32_t width, uint32_t height, uint8_t arrays)
{
    CloseGPU();
    SetupTexture(description.format, width, height, arrays);
}

template <unsigned int D>
inline std::vector<uint8_t>& Texture<D>::AcquireTextureBuffer()
{
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
    return AcquireTextureBuffer().data();
}

}
