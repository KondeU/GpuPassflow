#pragma once

// The macro was defined to disable msvc deprecation alerts for codecvt,
// which is currently marked as deprecated by the c++ spec, but c++ does
// not provide a new and stable one to replace it.
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>
#include <fstream>

namespace au::gp {

template <typename T>
inline constexpr typename std::underlying_type<T>::type EnumCast(const T& value)
{
    return static_cast<std::underlying_type<T>::type>(value);
}

}

namespace au::rhi {

enum class BasicFormat {
    R32G32B32A32_FLOAT,
    R32G32B32A32_UINT,
    R32G32B32A32_SINT,
    R32G32B32_FLOAT,
    R32G32B32_UINT,
    R32G32B32_SINT,
    R16G16B16A16_FLOAT,
    R16G16B16A16_UINT,
    R16G16B16A16_SINT,
    R16G16B16A16_UNORM,
    R16G16B16A16_SNORM,
    R32G32_FLOAT,
    R32G32_UINT,
    R32G32_SINT,
    R10G10B10A2_UINT,
    R10G10B10A2_UNORM,
    R8G8B8A8_UINT,
    R8G8B8A8_SINT,
    R8G8B8A8_UNORM,
    R8G8B8A8_UNORM_SRGB,
    R8G8B8A8_SNORM,
    R16G16_FLOAT,
    R16G16_UINT,
    R16G16_SINT,
    R16G16_UNORM,
    R16G16_SNORM,
    R32_FLOAT,
    R32_UINT,
    R32_SINT,
    R8G8_UINT,
    R8G8_SINT,
    R8G8_UNORM,
    R8G8_SNORM,
    R16_FLOAT,
    R16_UINT,
    R16_SINT,
    R16_UNORM,
    R16_SNORM,
    R8_UINT,
    R8_SINT,
    R8_UNORM,
    R8_SNORM,
    D32_FLOAT,
    D24_UNORM_S8_UINT,
    D16_UNORM,
    B5G6R5_UNORM,
    B5G5R5A1_UNORM,
    B8G8R8A8_UNORM,
    B8G8R8X8_UNORM,
    B8G8R8A8_UNORM_SRGB,
    B8G8R8X8_UNORM_SRGB
};

enum class IndexFormat {
    UINT16,
    UINT32
};

enum class VertexFormat {
    UINT8,
    UINT8_VEC2,
    UINT8_VEC3,
    UINT8_VEC4,
    SINT8,
    SINT8_VEC2,
    SINT8_VEC3,
    SINT8_VEC4,
    UINT16,
    UINT16_VEC2,
    UINT16_VEC3,
    UINT16_VEC4,
    SINT16,
    SINT16_VEC2,
    SINT16_VEC3,
    SINT16_VEC4,
    FLOAT16,
    FLOAT16_VEC2,
    FLOAT16_VEC3,
    FLOAT16_VEC4,
    UINT32,
    UINT32_VEC2,
    UINT32_VEC3,
    UINT32_VEC4,
    SINT32,
    SINT32_VEC2,
    SINT32_VEC3,
    SINT32_VEC4,
    FLOAT32,
    FLOAT32_VEC2,
    FLOAT32_VEC3,
    FLOAT32_VEC4
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
    COPY_DESTINATION
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
    UniformBuffer    = (1 << 0), // HW Type is UniformBuffer
    ReadOnlyBuffer   = (1 << 1), // HW Type is StorageBuffer
    ReadWriteBuffer  = (1 << 2), // HW Type is StorageBuffer
    ReadOnlyTexture  = (1 << 3), // HW Type is ImageBuffer
    ReadWriteTexture = (1 << 4), // HW Type is ImageBuffer
    ShaderResource   = // descriptor for shader resource: buffer or texture
        UniformBuffer | ReadOnlyBuffer | ReadWriteBuffer | ReadOnlyTexture | ReadWriteTexture,
    ImageSampler     = (1 << 5), // descriptor for image sampler
    ColorOutput      = (1 << 6), // descriptor for render target
    DepthStencil     = (1 << 7)  // descriptor for depth stencil
};

enum class PipelineStage : uint32_t {
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
    Vertex   = gp::EnumCast(PipelineStage::VertexShader),
    Hull     = gp::EnumCast(PipelineStage::HullShader),
    Domain   = gp::EnumCast(PipelineStage::DomainShader),
    Geometry = gp::EnumCast(PipelineStage::GeometryShader),
    Pixel    = gp::EnumCast(PipelineStage::PixelShader),
    Compute  = gp::EnumCast(PipelineStage::ComputeShader),
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
    BeginAction = Discard | Load | Clear,
    EndAction   = Discard | Store
};

inline bool IsBasicFormatHasDepth(BasicFormat format)
{
    switch (format) {
    case BasicFormat::D32_FLOAT:
    case BasicFormat::D24_UNORM_S8_UINT:
    case BasicFormat::D16_UNORM:
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
    case BasicFormat::R32G32B32A32_FLOAT:
    case BasicFormat::R32G32B32A32_UINT:
    case BasicFormat::R32G32B32A32_SINT:
        return 16;
    case BasicFormat::R32G32B32_FLOAT:
    case BasicFormat::R32G32B32_UINT:
    case BasicFormat::R32G32B32_SINT:
        return 12;
    case BasicFormat::R16G16B16A16_FLOAT:
    case BasicFormat::R16G16B16A16_UINT:
    case BasicFormat::R16G16B16A16_SINT:
    case BasicFormat::R16G16B16A16_UNORM:
    case BasicFormat::R16G16B16A16_SNORM:
        return 8;
    case BasicFormat::R32G32_FLOAT:
    case BasicFormat::R32G32_UINT:
    case BasicFormat::R32G32_SINT:
        return 8;
    case BasicFormat::R10G10B10A2_UINT:
    case BasicFormat::R10G10B10A2_UNORM:
        return 4;
    case BasicFormat::R8G8B8A8_UINT:
    case BasicFormat::R8G8B8A8_SINT:
    case BasicFormat::R8G8B8A8_UNORM:
    case BasicFormat::R8G8B8A8_UNORM_SRGB:
    case BasicFormat::R8G8B8A8_SNORM:
        return 4;
    case BasicFormat::R16G16_FLOAT:
    case BasicFormat::R16G16_UINT:
    case BasicFormat::R16G16_SINT:
    case BasicFormat::R16G16_UNORM:
    case BasicFormat::R16G16_SNORM:
        return 4;
    case BasicFormat::R32_FLOAT:
    case BasicFormat::R32_UINT:
    case BasicFormat::R32_SINT:
        return 4;
    case BasicFormat::R8G8_UINT:
    case BasicFormat::R8G8_SINT:
    case BasicFormat::R8G8_UNORM:
    case BasicFormat::R8G8_SNORM:
        return 2;
    case BasicFormat::R16_FLOAT:
    case BasicFormat::R16_UINT:
    case BasicFormat::R16_SINT:
    case BasicFormat::R16_UNORM:
    case BasicFormat::R16_SNORM:
        return 2;
    case BasicFormat::R8_UINT:
    case BasicFormat::R8_SINT:
    case BasicFormat::R8_UNORM:
    case BasicFormat::R8_SNORM:
    case BasicFormat::A8_UNORM:
        return 1;
    case BasicFormat::D32_FLOAT:
    case BasicFormat::D24_UNORM_S8_UINT:
        return 4;
    case BasicFormat::D16_UNORM:
        return 2;
    case BasicFormat::B5G6R5_UNORM:
    case BasicFormat::B5G5R5A1_UNORM:
        return 2;
    case BasicFormat::B8G8R8A8_UNORM:
    case BasicFormat::B8G8R8X8_UNORM:
    case BasicFormat::B8G8R8A8_UNORM_SRGB:
    case BasicFormat::B8G8R8X8_UNORM_SRGB:
        return 4;
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
    case VertexFormat::UINT8:
    case VertexFormat::SINT8:
        return 1;
    case VertexFormat::UINT8_VEC2:
    case VertexFormat::SINT8_VEC2:
        return 2;
    case VertexFormat::UINT8_VEC3:
    case VertexFormat::SINT8_VEC3:
        return 4;
    case VertexFormat::UINT8_VEC4:
    case VertexFormat::SINT8_VEC4:
        return 4;

    case VertexFormat::UINT16:
    case VertexFormat::SINT16:
    case VertexFormat::FLOAT16:
        return 2;
    case VertexFormat::UINT16_VEC2:
    case VertexFormat::SINT16_VEC2:
    case VertexFormat::FLOAT16_VEC2:
        return 4;
    case VertexFormat::UINT16_VEC3:
    case VertexFormat::SINT16_VEC3:
    case VertexFormat::FLOAT16_VEC3:
        return 8;
    case VertexFormat::UINT16_VEC4:
    case VertexFormat::SINT16_VEC4:
    case VertexFormat::FLOAT16_VEC4:
        return 8;

    case VertexFormat::UINT32:
    case VertexFormat::SINT32:
    case VertexFormat::FLOAT32:
        return 4;
    case VertexFormat::UINT32_VEC2:
    case VertexFormat::SINT32_VEC2:
    case VertexFormat::FLOAT32_VEC2:
        return 8;
    case VertexFormat::UINT32_VEC3:
    case VertexFormat::SINT32_VEC3:
    case VertexFormat::FLOAT32_VEC3:
        return 12;
    case VertexFormat::UINT32_VEC4:
    case VertexFormat::SINT32_VEC4:
    case VertexFormat::FLOAT32_VEC4:
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

namespace std {

#if defined(UNICODE) || defined(_UNICODE)
using tstring = wstring;
#else
using tstring = string;
#endif

inline wstring string_to_wstring(const string& str)
{
    using convert_type = codecvt_utf8<wchar_t>;
    wstring_convert<convert_type, wchar_t> cv;
    return cv.from_bytes(str);
}

inline string wstring_to_string(const wstring& wstr)
{
    using convert_type = codecvt_utf8<wchar_t>;
    wstring_convert<convert_type, wchar_t> cv;
    return cv.to_bytes(wstr);
}

inline wstring to_wstring(const string& str)
{
    return string_to_wstring(str);
}

inline wstring to_wstring(const wstring& wstr)
{
    return wstr;
}

inline string to_string(const wstring& wstr)
{
    return wstring_to_string(wstr);
}

inline string to_string(const string& str)
{
    return str;
}

}

namespace au::gp {

inline std::string ReadFile(const std::string& file)
{
    std::ifstream input(file);
    if (input.is_open()) {
        return std::string(
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>());
    }
    return {};
}

inline bool WriteFile(const std::string& file, const std::string& content)
{
    std::ofstream output(file);
    if (output.is_open()) {
        std::copy(content.begin(), content.end(),
            std::ostreambuf_iterator<char>(output));
        return true;
    }
    return false;
}

}
