#pragma once

#include "framework/types/TypeCast.hpp"

namespace au {

enum class BasicFormat {
    R8G8B8A8_UNORM,
    R32G32B32_FLOAT,
    R32G32B32A32_FLOAT,
    D24_UNORM_S8_UINT,
    // ...
    V32_FLOAT,
    V32V32_FLOAT,
    V32V32V32_FLOAT,
    V32V32V32V32_FLOAT
};

enum class IndexFormat {
    UINT16,
    UINT32
};

enum class VertexFormat {
    FLOAT32x3,
    FLOAT32x4
    // ...
};

enum class VertexInputRate {
    PER_VERTEX,
    PER_INSTANCE
};

enum class IndexStripCutValue {
    NONE_OR_DISABLE,
    UINT16_MAX_VALUE,
    UINT32_MAX_VALUE

};

enum class PrimitiveTopology {
    POINT_LIST,
    LINE_LIST,
    LINE_STRIP,
    TRIANGLE_LIST,
    TRIANGLE_STRIP
};

enum class TransferDirection {
    GPU_ONLY,
    CPU_TO_GPU,
    GPU_TO_CPU
};

enum class ResourceState {
    UNDEFINED,
    PRESENT,
    COLOR_OUTPUT,
    GENERAL_READ,
    GENERAL_READ_WRITE,
    DEPTH_STENCIL_READ,
    DEPTH_STENCIL_WRITE,
    COPY_SOURCE,
    COPY_DESTINATION,
    RESOLVE_SOURCE,
    RESOLVE_DESTINATION
};

enum class MSAA : uint8_t {
    MSAAx1 = 1,
    MSAAx4 = 4
};

enum class CommandType : uint8_t {
    Graphics = (1 << 0),
    Transfer = (1 << 1),
    Compute  = (1 << 2),
    Generic  = Graphics | Compute,
    All      = Graphics | Transfer | Compute
};

enum class ImageType : uint8_t {
    Color   = (1 << 0), // render target / color attachment
    Depth   = (1 << 1), // depth, a part of depth stencil attachment
    Stencil = (1 << 2), // stencil, a part of depth stencil attachment
    DepthStencil = Depth | Stencil, // depth stencil / depth stencil attachment
    ShaderResource = 0 // generic image, only can be used as shader resource, not as attachment
};

enum class ImageDimension : uint8_t {
    Dimension1D = 1, // Texture 1D
    Dimension2D = 2, // Texture 2D
    Dimension3D = 3  // Texture 3D
};

enum class DescriptorType : uint8_t {
    ConstantBuffer   = (1 << 0),
    StorageBuffer    = (1 << 1),
    ReadWriteBuffer  = (1 << 2),
    ReadOnlyTexture  = (1 << 3),
    ReadWriteTexture = (1 << 4),
    ShaderResource = // descriptor for shader resource: buffer or texture
        ConstantBuffer | StorageBuffer | ReadWriteBuffer | ReadOnlyTexture | ReadWriteTexture,
    ImageSampler   = (1 << 5), // descriptor for image sampler
    ColorOutput    = (1 << 6), // descriptor for render target
    DepthStencil   = (1 << 7)  // descriptor for depth stencil
};

enum class Stage : uint32_t {
    // Sort them by their order in the graphics pipeline:
    InputAssembler = (1 <<  0), // Standard stage: input vertex and index.
    VertexShader   = (1 <<  2), // Standard stage: vertex process.
    HullShader     = (1 <<  4), // Extra stage: hull process, determining how much an input control
                                // patch should be tessellated by the tessellation stage.
    Tessellation   = (1 <<  8), // Extra stage: tessellation, which converts low-detail subdivision
                                // surfaces into higher-detail primitives on the GPU.
    DomainShader   = (1 << 10), // Extra stage: domain process, compute the final vertex attributes
                                // based on the output control points from the hull shader and the
                                // interpolation coordinates from the tessellation stage.
    GeometryShader = (1 << 12), // Extra stage: geometry process, which runs shader code with vert-
                                // ices as input and the ability to generate vertices on output.
    StreamOutput   = (1 << 14), // Extra stage: output vertex data from the geometry-shader stage
                                // (or the vertex-shader stage if the geometry-shader stage is
                                // inactive) to one or more buffers in memory.
    Rasterizer     = (1 << 16), // Standard stage: rasterize.
    PixelShader    = (1 << 18), // Standard stage: pixel/fragment process.
    OutputMerger   = (1 << 20), // Standard stage: merge and output.
    ComputeShader  = (1 << 24), // Advance stage: compute pipeline process.
    // Programmable shader stage mask:
    ShaderStageMask = VertexShader | HullShader | DomainShader | GeometryShader |
                      PixelShader  | ComputeShader
};

enum class ShaderStage : uint32_t {
    Vertex   = framework::EnumCast(Stage::VertexShader),
    Hull     = framework::EnumCast(Stage::HullShader),
    Domain   = framework::EnumCast(Stage::DomainShader),
    Geometry = framework::EnumCast(Stage::GeometryShader),
    Pixel    = framework::EnumCast(Stage::PixelShader),
    Compute  = framework::EnumCast(Stage::ComputeShader),
    Graphics = Vertex | Hull | Domain | Geometry | Pixel
};

union ClearValue {
    struct Image {
        float color[4]{};
        float depth = 1.0f;
        uint8_t stencil = 0u;
    } image;
    struct Buffer {
        float value[4]{};
    } buffer;
};

struct Viewport {
    float x        = 0.0f; // left top x
    float y        = 0.0f; // left top y
    float width    = 0.0f; // right bottom x = x + width
    float height   = 0.0f; // right bottom y = y + height
    float minDepth = 0.0f; // depth mapping to:
    float maxDepth = 1.0f; // [minDepth, maxDepth]
};

struct Scissor {
    long left   = 0;
    long top    = 0;
    long right  = 0;
    long bottom = 0;
};

enum class FillMode {
    Solid,
    Wireframe
};

enum class CullMode {
    None,
    Front,
    Back
};

struct RasterizerState {
    FillMode fillMode;
    CullMode cullMode;
};

enum class AddressMode {
    Wrap,
    Mirror,
    Clamp,
    Border
};

struct SamplerState {
    enum class Filter {
        Point,      // Point and Linear can be used for different filter objects, for example,
        Linear,     // Minification uses the Point and Magnification uses the Linear filter.
        Anisotropic // But if use Anisotropic, must make sure Min/Mag/Mip all use it.
    } minification, magnification, mipLevel;
    AddressMode addressMode[3]; // UVW
    unsigned int maxAnisotropy; // If Filter is Anisotropic, maxAnisotropy will be used.
    float borderColor[4];       // If AddressMode is Border, borderColor will be used.
};

enum class PassAction : uint8_t {
    Discard = (1 << 0),
    Load    = (1 << 1),
    Store   = (1 << 2),
    Clear   = (1 << 3),
    Resolve = (1 << 4),
    BeginAction = Discard | Load  | Clear,
    EndAction   = Discard | Store | Resolve
};

inline bool IsBasicFormatHasDepth(BasicFormat format)
{
    switch (format) {
    case BasicFormat::D24_UNORM_S8_UINT:
        return true;
    }
    return false;
}

inline bool IsBasicFormatHasStencil(BasicFormat format)
{
    switch (format) {
    case BasicFormat::D24_UNORM_S8_UINT:
        return true;
    }
    return false;
}

inline unsigned int QueryBasicFormatBytes(BasicFormat format)
{
    switch (format) {
    case BasicFormat::R8G8B8A8_UNORM:
    case BasicFormat::D24_UNORM_S8_UINT:
    case BasicFormat::V32_FLOAT:
        return 4;
    case BasicFormat::V32V32_FLOAT:
        return 8;
    case BasicFormat::R32G32B32_FLOAT:
    case BasicFormat::V32V32V32_FLOAT:
        return 12;
    case BasicFormat::R32G32B32A32_FLOAT:
    case BasicFormat::V32V32V32V32_FLOAT:
        return 16;
    }
    return 0;
}

inline unsigned int QueryIndexFormatBytes(IndexFormat format)
{
    switch (format) {
    case IndexFormat::UINT16:
        return 2;
    case IndexFormat::UINT32:
        return 4;
    }
    return 0;
}

inline unsigned int QueryVertexFormatBytes(VertexFormat format)
{
    switch (format) {
    case VertexFormat::FLOAT32x3:
        return 12;
    case VertexFormat::FLOAT32x4:
        return 16;
    }
    return 0;
}

inline ClearValue MakeImageClearValue(float c1, float c2, float c3, float c4)
{
    ClearValue clear{};
    clear.image.color[0] = c1;
    clear.image.color[1] = c2;
    clear.image.color[2] = c3;
    clear.image.color[3] = c4;
    return clear;
}

inline ClearValue MakeImageClearValue(float depth, uint8_t stencil)
{
    ClearValue clear{};
    clear.image.depth = depth;
    clear.image.stencil = stencil;
    return clear;
}

inline ClearValue MakeBufferClearValue(float b1, float b2, float b3, float b4)
{
    ClearValue clear{};
    clear.buffer.value[0] = b1;
    clear.buffer.value[1] = b2;
    clear.buffer.value[2] = b3;
    clear.buffer.value[3] = b4;
    return clear;
}

}
