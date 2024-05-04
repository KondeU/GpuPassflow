#pragma once

#include <unordered_map>
#include "Resources.h"

// Support custom hash key :-)
#ifndef GP_OPT_FRAME_RESOURCES_KEY_TYPE
#define GP_OPT_FRAME_RESOURCES_KEY_TYPE std::string
#endif

namespace au::gp {

using FRsKey = GP_OPT_FRAME_RESOURCES_KEY_TYPE;

struct ResourceContainer final {
    std::unordered_map<FRsKey, Resource<BaseConstantBuffer>> constantBuffers;
    std::unordered_map<FRsKey, Resource<BaseStructuredBuffer>> structuredBuffers;
    std::unordered_map<FRsKey, Resource<BaseTexture>> textures;
    std::unordered_map<FRsKey, Resource<Sampler>> samplers;
};

struct OutputContainer final {
    std::unordered_map<FRsKey, Resource<ColorOutput>> colorOutputs;
    std::unordered_map<FRsKey, Resource<DepthStencilOutput>> depthStencilOutputs;
    std::unordered_map<FRsKey, Resource<DisplayPresentOutput>> displayPresentOutputs;
};

struct DrawItem final {
    Resource<BaseIndexBuffer> indexBuffer;
    Resource<BaseVertexBuffer> vertexBuffer;
    ResourceContainer objectResources;
};

struct DispatchItem final {
    unsigned int threadGroups[3] = { 1u, 1u, 1u };
    ResourceContainer objectResources;
};

struct ViewResources final {
    OutputContainer viewOutputs;
    ResourceContainer viewResources;
};

struct SceneResources final {
    ResourceContainer sceneResources;
    std::vector<std::shared_ptr<DrawItem>> drawItems;
    std::vector<std::shared_ptr<DispatchItem>> dispatchItems;
    std::unordered_map<FRsKey, ViewResources> viewsResources;
};

struct FrameResources final {
    ResourceContainer passResources;
    std::unordered_map<FRsKey, SceneResources> scenesResources;
};

}
