#pragma once

#include <unordered_map>
#include "Resources.h"

namespace au::gp {

struct ResourceContainer final {
    std::unordered_map<std::string, Resource<BaseConstantBuffer>> constantBuffers;
    std::unordered_map<std::string, Resource<BaseStructuredBuffer>> structuredBuffers;
    std::unordered_map<std::string, Resource<BaseTexture>> textures;
    std::unordered_map<std::string, Resource<Sampler>> samplers;
};

struct OutputContainer final {
    std::unordered_map<std::string, Resource<ColorOutput>> colorOutputs;
    std::unordered_map<std::string, Resource<DepthStencilOutput>> depthStencilOutputs;
    std::unordered_map<std::string, Resource<DisplayPresentOutput>> displayPresentOutputs;
};

struct DrawItem final {
    Resource<BaseIndexBuffer> indexBuffer;
    Resource<BaseVertexBuffer> vertexBuffer;
    ResourceContainer itemResources;
};

struct DispatchItem final {
    unsigned int threadGroups[3] = { 1u, 1u, 1u };
    ResourceContainer itemResources;
};

struct FrameResources final {
    OutputContainer frameOutputs;
    ResourceContainer frameResources;
    std::vector<Resource<DrawItem>> drawItems;
    std::vector<Resource<DispatchItem>> dispatchItems;
};

}
