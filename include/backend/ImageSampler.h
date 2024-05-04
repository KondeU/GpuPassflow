#pragma once

#include "BasicTypes.h"

namespace au::rhi {

class ImageSampler {
public:
    struct Description final {
        SamplerState state;

        Description(SamplerState::Filter filterForAll, AddressMode addressModeForAll)
        {
            state.minification = filterForAll;
            state.magnification = filterForAll;
            state.mipLevel = filterForAll;
            state.addressMode[0] = addressModeForAll;
            state.addressMode[1] = addressModeForAll;
            state.addressMode[2] = addressModeForAll;
            state.maxAnisotropy = 16u; // Default value is maximum.
            state.borderColor[0] = 0.0f;
            state.borderColor[1] = 0.0f;
            state.borderColor[2] = 0.0f;
            state.borderColor[3] = 0.0f;
        }

        Description(
            SamplerState::Filter minificationFilter,
            SamplerState::Filter magnificationFilter,
            SamplerState::Filter mipLevelFilter,
            AddressMode addressU,
            AddressMode addressV,
            AddressMode addressW,
            unsigned int maxAnisotropy = 16u,
            float borderColor0 = 0.0f,
            float borderColor1 = 0.0f,
            float borderColor2 = 0.0f,
            float borderColor3 = 0.0f)
        {
            state.minification = minificationFilter;
            state.magnification = magnificationFilter;
            state.mipLevel = mipLevelFilter;
            state.addressMode[0] = addressU;
            state.addressMode[1] = addressV;
            state.addressMode[2] = addressW;
            state.maxAnisotropy = maxAnisotropy;
            state.borderColor[0] = borderColor0;
            state.borderColor[1] = borderColor1;
            state.borderColor[2] = borderColor2;
            state.borderColor[3] = borderColor3;
        }
    };

protected:
    ImageSampler() = default;
    virtual ~ImageSampler() = default;
};

}
