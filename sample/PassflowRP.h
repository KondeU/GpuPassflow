#pragma once

#include "PassflowDemo.h"

class PassflowRP : public PassflowDemo {
public:
    PassflowRP() = default;
    virtual ~PassflowRP();

protected:
    void Setup() override;
    void ExecuteOneFrame() override;
    void SizeChanged(void* window, unsigned int width, unsigned int height) override;

    void AutomateRotate();
    void UpdateData();
    void Draw();

private:
    GP_LOG_TAG(PassflowRP);

    float aspectRatio = 0.0f;

    float theta = 1.5f * DirectX::XM_PI;
    float phi = DirectX::XM_PIDIV4;
    float radius = 5.0f;
    float delta = 0.01f;

    static const std::vector<uint16_t> indices;
    static const std::vector<DirectX::XMFLOAT3> positions;
    static const std::vector<DirectX::XMFLOAT4> colors;

    struct ObjectMVP {
        DirectX::XMFLOAT4X4 mvp;
    };

    std::unique_ptr<au::gp::Passflow> passflow;

    au::gp::RasterizePass* drawPass = nullptr;
    au::gp::RasterizePass* presentPass = nullptr;

    au::gp::Resource<au::gp::VertexBuffer<DirectX::XMFLOAT3>> cubeVertices;
    au::gp::Resource<au::gp::IndexBuffer<uint32_t>> cubeIndices;

    au::gp::Resource<au::gp::StructuredBuffer<DirectX::XMFLOAT4>> cubeVerticesColors;

    au::gp::Resource<au::gp::ConstantBuffer<ObjectMVP>> cubeMVP;

    au::gp::Resource<au::gp::Texture2D> sampledTexture;
    au::gp::Resource<au::gp::Sampler> textureSampler;

    au::gp::Resource<au::gp::DisplayPresentOutput> displayOutput;
    au::gp::Resource<au::gp::DepthStencilOutput> depthStencilOutput;
    au::gp::Resource<au::gp::ColorOutput> presentColorOutput;
    au::gp::Resource<au::gp::ColorOutput> halfColorOutput;

    unsigned int frameIndex = 0;
};
