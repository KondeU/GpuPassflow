#pragma once

#include <chrono>
#include "PassflowDemo.h"

class PassflowCP : public PassflowDemo {
public:
    PassflowCP() = default;
    virtual ~PassflowCP();

protected:
    void Setup() override;
    void ExecuteOneFrame() override;
    void SizeChanged(void* window, unsigned int width, unsigned int height) override;

private:
    GP_LOG_TAG(PassflowCP);

    std::unique_ptr<au::gp::Passflow> passflow;

    au::gp::ComputePass* computePass = nullptr;
    au::gp::RasterizePass* presentPass = nullptr;

    au::gp::Resource<au::gp::DisplayPresentOutput> outputDisplay;
    au::gp::Resource<au::gp::Texture2D> outputColor;

    std::chrono::steady_clock::time_point timestamp = std::chrono::steady_clock::now();
    struct FDBRProperties final {
        /** Shader input in ShaderToy:
         *  uniform vec3 iResolution;            // viewport resolution (in pixels)
         *  uniform float iTime;                 // shader playback time (in seconds)
         *  uniform float iTimeDelta;            // render time (in seconds)
         *  uniform float iFrameRate;            // shader frame rate
         *  uniform int iFrame;                  // shader playback frame
         *  uniform float iChannelTime[4];       // channel playback time (in seconds)
         *  uniform vec3 iChannelResolution[4];  // channel resolution (in pixels)
         *  uniform vec4 iMouse;                 // mouse pixel coords.
         *                                          xy: current (if MLB down), zw: click
         *  uniform samplerXX iChannel0..3;      // input channel. XX = 2D/Cube
         *  uniform vec4 iDate;                  // (year, month, day, time in seconds)
         *  uniform float iSampleRate;           // sound sample rate (i.e., 44100)
        */
        float width;    // output color attachment width (in pixels)
        float height;   // output color attachment height (in pixels)
        float time;     // shader playback time (in seconds)
        float delta;    // render time (in seconds)
        float frames;   // shader playback frame (frames count)
        float fps;      // shader frame rate (frame per second)
        float mouseX;   // mouse X coord (pixel)
        float mouseY;   // mouse Y coord (pixel)
        int mouseState; // mouse state
                        //   32bits: [ 8bits(3) | 8bits(2) | 8bits(1) | 8bits(0) ]:
                        //   (0) [7][6][5][4][3][2][1][0]
                        //      [0]: mouse moving
                        //   (1) [7][6][5][4][3][2][1][0]
                        //      [0]: left button pressing
                        //      [1]: left button double click
                        //   (2) [7][6][5][4][3][2][1][0]
                        //      [0]: right button pressing
                        //      [1]: right button double click
        int reserved[3];
    };
    au::gp::Resource<au::gp::ConstantBuffer<FDBRProperties>> inputProperties;
    au::gp::Resource<au::gp::Texture3D> input2DTexturesArray; // reserved
    au::gp::Resource<au::gp::Sampler> inputTextureSampler;

    std::shared_ptr<au::gp::DispatchItem> dispatchItem;

    unsigned int frameIndex = 0;
};
