#include "DX12BasicTypes.h"

namespace au::backend {

DXGI_FORMAT ConvertBasicFormat(BasicFormat format)
{
    static const std::unordered_map<BasicFormat, DXGI_FORMAT> map = {
        { BasicFormat::R8G8B8A8_UNORM,     DXGI_FORMAT_R8G8B8A8_UNORM     },
        { BasicFormat::R32G32B32_FLOAT,    DXGI_FORMAT_R32G32B32_FLOAT    },
        { BasicFormat::R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
        { BasicFormat::D24_UNORM_S8_UINT,  DXGI_FORMAT_D24_UNORM_S8_UINT  },
        { BasicFormat::V32_FLOAT,          DXGI_FORMAT_R32_FLOAT          },
        { BasicFormat::V32V32_FLOAT,       DXGI_FORMAT_R32G32_FLOAT       },
        { BasicFormat::V32V32V32_FLOAT,    DXGI_FORMAT_R32G32B32_FLOAT    },
        { BasicFormat::V32V32V32V32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT }
    };
    return map.at(format);
}

DXGI_FORMAT ConvertIndexFormat(IndexFormat format)
{
    static const std::unordered_map<IndexFormat, DXGI_FORMAT> map = {
        { IndexFormat::UINT16, DXGI_FORMAT_R16_UINT },
        { IndexFormat::UINT32, DXGI_FORMAT_R32_UINT }
    };
    return map.at(format);
}

DXGI_FORMAT ConvertVertexFormat(VertexFormat format)
{
    static const std::unordered_map<VertexFormat, DXGI_FORMAT> map = {
        { VertexFormat::FLOAT32x3, DXGI_FORMAT_R32G32B32_FLOAT    },
        { VertexFormat::FLOAT32x4, DXGI_FORMAT_R32G32B32A32_FLOAT }
    };
    return map.at(format);
}

D3D12_INPUT_CLASSIFICATION ConvertInputClassification(VertexInputRate rate)
{
    static const std::unordered_map<VertexInputRate, D3D12_INPUT_CLASSIFICATION> map = {
        { VertexInputRate::PER_VERTEX,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA   },
        { VertexInputRate::PER_INSTANCE, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA }
    };
    return map.at(rate);
}

D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ConvertStripValue(IndexStripCutValue value)
{
    static const std::unordered_map<IndexStripCutValue, D3D12_INDEX_BUFFER_STRIP_CUT_VALUE> map = {
        { IndexStripCutValue::NONE_OR_DISABLE,  D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED   },
        { IndexStripCutValue::UINT16_MAX_VALUE, D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF     },
        { IndexStripCutValue::UINT32_MAX_VALUE, D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF }
    };
    return map.at(value);
}

D3D12_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(PrimitiveTopology topology)
{
    static const std::unordered_map<PrimitiveTopology, D3D12_PRIMITIVE_TOPOLOGY> map = {
        { PrimitiveTopology::POINT_LIST,     D3D_PRIMITIVE_TOPOLOGY_POINTLIST     },
        { PrimitiveTopology::LINE_LIST,      D3D_PRIMITIVE_TOPOLOGY_LINELIST      },
        { PrimitiveTopology::LINE_STRIP,     D3D_PRIMITIVE_TOPOLOGY_LINESTRIP     },
        { PrimitiveTopology::TRIANGLE_LIST,  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST  },
        { PrimitiveTopology::TRIANGLE_STRIP, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP }
    };
    return map.at(topology);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(PrimitiveTopology topology)
{
    static const std::unordered_map<PrimitiveTopology, D3D12_PRIMITIVE_TOPOLOGY_TYPE> map = {
        { PrimitiveTopology::POINT_LIST,     D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT    },
        { PrimitiveTopology::LINE_LIST,      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     },
        { PrimitiveTopology::LINE_STRIP,     D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     },
        { PrimitiveTopology::TRIANGLE_LIST,  D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE },
        { PrimitiveTopology::TRIANGLE_STRIP, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE }
    };
    return map.at(topology);
}

D3D12_HEAP_TYPE ConvertHeap(TransferDirection type)
{
    static const std::unordered_map<TransferDirection, D3D12_HEAP_TYPE> map = {
        { TransferDirection::GPU_ONLY,   D3D12_HEAP_TYPE_DEFAULT  },
        { TransferDirection::CPU_TO_GPU, D3D12_HEAP_TYPE_UPLOAD   },
        { TransferDirection::GPU_TO_CPU, D3D12_HEAP_TYPE_READBACK }
    };
    return map.at(type);
}

D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state)
{
    static const std::unordered_map<ResourceState, D3D12_RESOURCE_STATES> map = {
        { ResourceState::UNDEFINED,           D3D12_RESOURCE_STATE_COMMON           },
        { ResourceState::PRESENT,             D3D12_RESOURCE_STATE_PRESENT          },
        { ResourceState::COLOR_OUTPUT,        D3D12_RESOURCE_STATE_RENDER_TARGET    },
        { ResourceState::GENERAL_READ,        D3D12_RESOURCE_STATE_GENERIC_READ     },
        { ResourceState::GENERAL_READ_WRITE,  D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
        { ResourceState::DEPTH_STENCIL_READ,  D3D12_RESOURCE_STATE_DEPTH_READ       },
        { ResourceState::DEPTH_STENCIL_WRITE, D3D12_RESOURCE_STATE_DEPTH_WRITE      },
        { ResourceState::COPY_SOURCE,         D3D12_RESOURCE_STATE_COPY_SOURCE      },
        { ResourceState::COPY_DESTINATION,    D3D12_RESOURCE_STATE_COPY_DEST        },
        { ResourceState::RESOLVE_SOURCE,      D3D12_RESOURCE_STATE_RESOLVE_SOURCE   },
        { ResourceState::RESOLVE_DESTINATION, D3D12_RESOURCE_STATE_RESOLVE_DEST     }
    };
    return map.at(state);
}

unsigned int ConvertMSAA(MSAA msaa)
{
    return framework::EnumCast(msaa);
}

D3D12_RESOURCE_FLAGS ConvertImageResourceFlag(ImageType type)
{
    static const std::unordered_map<ImageType, D3D12_RESOURCE_FLAGS> map = {
        { ImageType::Color,          D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET },
        { ImageType::Depth,          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        { ImageType::Stencil,        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        { ImageType::DepthStencil,   D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        { ImageType::ShaderResource, D3D12_RESOURCE_FLAG_NONE                }
    };
    return map.at(type);
}

D3D12_RESOURCE_DIMENSION ConvertImageDimension(ImageDimension dimension)
{
    static const std::unordered_map<ImageDimension, D3D12_RESOURCE_DIMENSION> map = {
        { ImageDimension::Dimension1D, D3D12_RESOURCE_DIMENSION_TEXTURE1D },
        { ImageDimension::Dimension2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D },
        { ImageDimension::Dimension3D, D3D12_RESOURCE_DIMENSION_TEXTURE3D }
    };
    return map.at(dimension);
}

D3D12_DESCRIPTOR_HEAP_TYPE ConvertDescriptorHeap(DescriptorType type)
{
    static const std::unordered_map<DescriptorType, D3D12_DESCRIPTOR_HEAP_TYPE> map = {
        { DescriptorType::ShaderResource, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV },
        { DescriptorType::ImageSampler,   D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER     },
        { DescriptorType::ColorOutput,    D3D12_DESCRIPTOR_HEAP_TYPE_RTV         },
        { DescriptorType::DepthStencil,   D3D12_DESCRIPTOR_HEAP_TYPE_DSV         }
    };
    if (framework::EnumCast(type) & framework::EnumCast(DescriptorType::ShaderResource)) {
        type = DescriptorType::ShaderResource;
    }
    return map.at(type);
}

D3D12_DESCRIPTOR_HEAP_FLAGS ConvertDescriptorHeapVisible(DescriptorType type)
{
    static const std::unordered_map<DescriptorType, D3D12_DESCRIPTOR_HEAP_FLAGS> map = {
        { DescriptorType::ShaderResource, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE },
        { DescriptorType::ImageSampler,   D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE },
        { DescriptorType::ColorOutput,    D3D12_DESCRIPTOR_HEAP_FLAG_NONE           },
        { DescriptorType::DepthStencil,   D3D12_DESCRIPTOR_HEAP_FLAG_NONE           }
    };
    if (framework::EnumCast(type) & framework::EnumCast(DescriptorType::ShaderResource)) {
        type = DescriptorType::ShaderResource;
    }
    return map.at(type);
}

D3D12_DESCRIPTOR_RANGE_TYPE ConvertDescriptorRangeType(DescriptorType type, bool& success)
{
    static const std::unordered_map<DescriptorType, D3D12_DESCRIPTOR_RANGE_TYPE> map = {
        { DescriptorType::ConstantBuffer,   D3D12_DESCRIPTOR_RANGE_TYPE_CBV     },
        { DescriptorType::StorageBuffer,    D3D12_DESCRIPTOR_RANGE_TYPE_SRV     },
        { DescriptorType::ReadWriteBuffer,  D3D12_DESCRIPTOR_RANGE_TYPE_UAV     },
        { DescriptorType::ReadOnlyTexture,  D3D12_DESCRIPTOR_RANGE_TYPE_SRV     },
        { DescriptorType::ReadWriteTexture, D3D12_DESCRIPTOR_RANGE_TYPE_UAV     },
        { DescriptorType::ImageSampler,     D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER }
    };
    success = false;
    if ((type == DescriptorType::ImageSampler) ||
        ((type != DescriptorType::ShaderResource) &&
            (framework::EnumCast(type) & framework::EnumCast(DescriptorType::ShaderResource)))) {
        success = true;
        return map.at(type);
    }
    return {};
}

D3D12_SHADER_VISIBILITY ConvertShaderVisibility(ShaderStage visibility)
{
    static const std::unordered_map<ShaderStage, D3D12_SHADER_VISIBILITY> map = {
        { ShaderStage::Vertex,   D3D12_SHADER_VISIBILITY_VERTEX   },
        { ShaderStage::Hull,     D3D12_SHADER_VISIBILITY_HULL     },
        { ShaderStage::Domain,   D3D12_SHADER_VISIBILITY_DOMAIN   },
        { ShaderStage::Geometry, D3D12_SHADER_VISIBILITY_GEOMETRY },
        { ShaderStage::Pixel,    D3D12_SHADER_VISIBILITY_PIXEL    },
        { ShaderStage::Compute,  D3D12_SHADER_VISIBILITY_ALL      },
        { ShaderStage::Graphics, D3D12_SHADER_VISIBILITY_ALL      }
    };
    return map.at(visibility);
}

D3D12_CLEAR_VALUE ConvertClearValue(BasicFormat format, ClearValue value)
{
    if (IsBasicFormatHasDepth(format) || IsBasicFormatHasStencil(format)) {
        return CD3DX12_CLEAR_VALUE(ConvertBasicFormat(format),
            value.image.depth, value.image.stencil);
    }
    return CD3DX12_CLEAR_VALUE(ConvertBasicFormat(format), value.image.color);
}

D3D12_CLEAR_FLAGS ConvertClearFlags(BasicFormat format)
{
    D3D12_CLEAR_FLAGS flags;
    ZeroMemory(&flags, sizeof(flags));
    if (IsBasicFormatHasDepth(format)) {
        flags |= D3D12_CLEAR_FLAG_DEPTH;
    }
    if (IsBasicFormatHasStencil(format)) {
        flags |= D3D12_CLEAR_FLAG_STENCIL;
    }
    return flags;
}

D3D12_FILL_MODE ConvertFillMode(FillMode mode)
{
    static const std::unordered_map<FillMode, D3D12_FILL_MODE> map = {
        { FillMode::Solid,     D3D12_FILL_MODE_SOLID     },
        { FillMode::Wireframe, D3D12_FILL_MODE_WIREFRAME }
    };
    return map.at(mode);
}

D3D12_CULL_MODE ConvertCullMode(CullMode mode)
{
    static const std::unordered_map<CullMode, D3D12_CULL_MODE> map = {
        { CullMode::None,  D3D12_CULL_MODE_NONE  },
        { CullMode::Front, D3D12_CULL_MODE_FRONT },
        { CullMode::Back,  D3D12_CULL_MODE_BACK  }
    };
    return map.at(mode);
}

D3D12_RASTERIZER_DESC ConvertRasterizerState(RasterizerState state)
{
    CD3DX12_RASTERIZER_DESC rasterizerState(D3D12_DEFAULT);
    rasterizerState.FillMode = ConvertFillMode(state.fillMode);
    rasterizerState.CullMode = ConvertCullMode(state.cullMode);
    return rasterizerState;
}

D3D12_TEXTURE_ADDRESS_MODE ConvertAddressMode(AddressMode mode)
{
    static const std::unordered_map<AddressMode, D3D12_TEXTURE_ADDRESS_MODE> map = {
        { AddressMode::Wrap,   D3D12_TEXTURE_ADDRESS_MODE_WRAP   },
        { AddressMode::Mirror, D3D12_TEXTURE_ADDRESS_MODE_MIRROR },
        { AddressMode::Clamp,  D3D12_TEXTURE_ADDRESS_MODE_CLAMP  },
        { AddressMode::Border, D3D12_TEXTURE_ADDRESS_MODE_BORDER }
    };
    return map.at(mode);
}

D3D12_SAMPLER_DESC ConvertSamplerState(SamplerState state)
{
    D3D12_SAMPLER_DESC samplerState{};
    if (state.minification  == SamplerState::Filter::Anisotropic ||
        state.magnification == SamplerState::Filter::Anisotropic ||
        state.mipLevel      == SamplerState::Filter::Anisotropic) {
        samplerState.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(0);
    } else {
        auto ConvertFilter = [](SamplerState::Filter filter) {
            if (filter == SamplerState::Filter::Point) {
                return D3D12_FILTER_TYPE_POINT;
            }
            // Else is SamplerState::Filter::Linear
            return D3D12_FILTER_TYPE_LINEAR;
        };
        samplerState.Filter = D3D12_ENCODE_BASIC_FILTER(
            ConvertFilter(state.minification),
            ConvertFilter(state.magnification),
            ConvertFilter(state.mipLevel),
            0);
    }
    samplerState.AddressU = ConvertAddressMode(state.addressMode[0]);
    samplerState.AddressV = ConvertAddressMode(state.addressMode[1]);
    samplerState.AddressW = ConvertAddressMode(state.addressMode[2]);
    samplerState.MipLODBias = 0; // TODO: Mip level filter is no supported yet.
    samplerState.MaxAnisotropy = state.maxAnisotropy;
    samplerState.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // TODO: No supported yet.
    samplerState.BorderColor[0] = state.borderColor[0];
    samplerState.BorderColor[1] = state.borderColor[1];
    samplerState.BorderColor[2] = state.borderColor[2];
    samplerState.BorderColor[3] = state.borderColor[3];
    samplerState.MinLOD = 0.0f;
    samplerState.MaxLOD = D3D12_FLOAT32_MAX;
    return samplerState;
}

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE ConvertRenderPassBeginningAccessType(PassAction action)
{
    static const std::unordered_map<PassAction, D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE> map = {
        { PassAction::Discard, D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD  },
        { PassAction::Load,    D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE },
        { PassAction::Clear,   D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR    }
    };
    if (framework::EnumCast(action) & framework::EnumCast(PassAction::BeginAction)) {
        return map.at(action);
    }
    return {};
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE ConvertRenderPassEndingAccessType(PassAction action)
{
    static const std::unordered_map<PassAction, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE> map = {
        { PassAction::Discard, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD  },
        { PassAction::Store,   D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE },
        { PassAction::Resolve, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE  }
    };
    if (framework::EnumCast(action) & framework::EnumCast(PassAction::EndAction)) {
        return map.at(action);
    }
    return {};
}

}
