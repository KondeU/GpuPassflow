#include "PassflowRP.h"

const std::string VertexShaderString = R"(
StructuredBuffer<float4> inColor : register(t0);

cbuffer cbPerObject : register(b1)
{
    float4x4 gModelViewProj;
};

struct VertexIn
{
    float3 inLocalPos : POSITION;
    uint   vertexId   : SV_VertexID;
};

struct VertexOut
{
    float4 outWorldPos : SV_POSITION;
    float4 outColor    : COLOR;
};

VertexOut Main(VertexIn vin)
{
    VertexOut vout;
    // Transform to homogeneous clip space.
    vout.outWorldPos = mul(float4(vin.inLocalPos, 1.0f), gModelViewProj);
    // Just pass vertex color into the pixel shader.
    vout.outColor = inColor[vin.vertexId];
    // Output to next pass.
    return vout;
}
)";

const std::string FragmentShaderString = R"(
Texture2D gHalfTexture : register(t0, space3);
SamplerState gSimpleSampler : register(s1, space3);

struct VertexOut
{
    float4 outWorldPos : SV_POSITION;
    float4 outColor    : COLOR;
};

struct PixelOut
{
    float4 color     : SV_Target0;
    float4 halfColor : SV_Target1;
};

PixelOut Main(VertexOut pin)
{
    PixelOut pout;
    pout.color = pin.outColor;
    pout.halfColor = pin.outColor * gHalfTexture.Sample(gSimpleSampler, float2(0.0f, 0.0f));
    return pout;
}
)";

const std::vector<DirectX::XMFLOAT3> CubeVertices = {
    DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),
    DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f),
    DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f),
    DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f),
    DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f),
    DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f),
    DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f),
    DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f)
};

const std::vector<uint32_t> CubeIndices = {
    // front face
    0, 1, 2,
    0, 2, 3,
    // back face
    4, 6, 5,
    4, 7, 6,
    // left face
    4, 5, 1,
    4, 1, 0,
    // right face
    3, 2, 6,
    3, 6, 7,
    // top face
    1, 5, 6,
    1, 6, 2,
    // bottom face
    4, 0, 3,
    4, 3, 7
};

const std::vector<DirectX::XMFLOAT4> CubeColors = {
    DirectX::XMFLOAT4(1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f), // White
    DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f), // Black
    DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f), // Red
    DirectX::XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f), // Green
    DirectX::XMFLOAT4(0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f), // Blue
    DirectX::XMFLOAT4(1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f), // Yellow
    DirectX::XMFLOAT4(0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f), // Cyan
    DirectX::XMFLOAT4(1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f)  // Magenta
};

class DrawPass : public au::gp::RasterizePass {
public:
    DrawPass(au::gp::Passflow& passflow);
    ~DrawPass() override;

protected:
    void OnPreparePass(au::rhi::Device* device) override;
    void OnBeforePass(unsigned int currentBufferingIndex) override;
    void OnExecutePass(au::rhi::CommandRecorder* recorder) override;
    void OnAfterPass(unsigned int currentPassInFlowIndex) override;
    void OnEnablePass(bool enable) override;

private:
    GP_LOG_TAG(DrawPass);

    unsigned int currentBufferingIndex = 0;
    std::unique_ptr<InputProperties> inputProperties;
    std::unique_ptr<OutputProperties> outputProperties;
    std::unique_ptr<ProgramProperties> programProperties;
    std::unique_ptr<ShaderResourceProperties> resourceProperties;
};

DrawPass::DrawPass(au::gp::Passflow& passflow) : RasterizePass(passflow)
{
}

DrawPass::~DrawPass()
{
}

void DrawPass::OnPreparePass(au::rhi::Device* device)
{
    InitializePipeline(device);

    inputProperties = std::make_unique<InputProperties>(InputProperties{
        { InputProperties::MakeDefaultPositionVertexAttribute() },
        { au::rhi::IndexFormat::UINT32,
          au::rhi::PrimitiveTopology::TRIANGLE_LIST,
          au::rhi::IndexStripCutValue::UINT32_MAX_VALUE },
        true // Support to add multiple draw items, default is true.
    });
    DeclareInput(*inputProperties);

    outputProperties = std::make_unique<OutputProperties>();
    outputProperties->targets[OutputProperties::OutputSlot::DS] = {
        au::rhi::BasicFormat::D24_UNORM_S8_UINT,     // imagePixelFormat
        au::rhi::PassAction::Clear,                  // beginAction
        au::rhi::PassAction::Store,                  // endAction
        au::rhi::ResourceState::GENERAL_READ,        // beforeState
        au::rhi::ResourceState::DEPTH_STENCIL_WRITE, // currentState
        au::rhi::ResourceState::GENERAL_READ         // afterState
    };
    outputProperties->targets[OutputProperties::OutputSlot::C0] = {
        au::rhi::BasicFormat::R8G8B8A8_UNORM,        // imagePixelFormat
        au::rhi::PassAction::Clear,                  // beginAction
        au::rhi::PassAction::Store,                  // endAction
        au::rhi::ResourceState::GENERAL_READ,        // beforeState
        au::rhi::ResourceState::COLOR_OUTPUT,        // currentState
        au::rhi::ResourceState::GENERAL_READ         // afterState
    };
    outputProperties->targets[OutputProperties::OutputSlot::C1] = {
        au::rhi::BasicFormat::R8G8B8A8_UNORM,        // imagePixelFormat
        au::rhi::PassAction::Clear,                  // beginAction
        au::rhi::PassAction::Store,                  // endAction
        au::rhi::ResourceState::GENERAL_READ,        // beforeState
        au::rhi::ResourceState::COLOR_OUTPUT,        // currentState
        au::rhi::ResourceState::GENERAL_READ         // afterState
    };
    DeclareOutput(*outputProperties);

    programProperties = std::make_unique<ProgramProperties>();
    programProperties->shaders[au::rhi::ShaderStage::Vertex] = { VertexShaderString, "Main" };
    programProperties->shaders[au::rhi::ShaderStage::Pixel] = { FragmentShaderString, "Main" };
    DeclareProgram(*programProperties);

    resourceProperties = std::make_unique<ShaderResourceProperties>();
    resourceProperties->resources[ShaderResourceProperties::ResourceSpace::PerObject] = {
        { // The object vertex color
            0,                                        // baseBindingPoint
            1,                                        // bindingPointCount
            au::rhi::ShaderStage::Graphics,           // resourceVisibility
            au::rhi::DescriptorType::StorageBuffer,   // resourceType
            au::rhi::ResourceState::GENERAL_READ,     // beforeState
            au::rhi::ResourceState::GENERAL_READ,     // currentState
            au::rhi::ResourceState::GENERAL_READ      // afterState
        },
        { // The object MVP matrix
            1,                                        // baseBindingPoint
            1,                                        // bindingPointCount
            au::rhi::ShaderStage::Graphics,           // resourceVisibility
            au::rhi::DescriptorType::ConstantBuffer,  // resourceType
            au::rhi::ResourceState::GENERAL_READ,     // beforeState
            au::rhi::ResourceState::GENERAL_READ,     // currentState
            au::rhi::ResourceState::GENERAL_READ      // afterState
        }
    };
    resourceProperties->resources[ShaderResourceProperties::ResourceSpace::PerPass] = {
        { // A test texture
            0,                                        // baseBindingPoint
            1,                                        // bindingPointCount
            au::rhi::ShaderStage::Graphics,           // resourceVisibility
            au::rhi::DescriptorType::ReadOnlyTexture, // resourceType
            au::rhi::ResourceState::GENERAL_READ,     // beforeState
            au::rhi::ResourceState::GENERAL_READ,     // currentState
            au::rhi::ResourceState::GENERAL_READ      // afterState
        },
        { // A texture sampler
            1,                                        // baseBindingPoint
            1,                                        // bindingPointCount
            au::rhi::ShaderStage::Graphics,           // resourceVisibility
            au::rhi::DescriptorType::ImageSampler,    // resourceType
            au::rhi::ResourceState::GENERAL_READ,     // beforeState
            au::rhi::ResourceState::GENERAL_READ,     // currentState
            au::rhi::ResourceState::GENERAL_READ      // afterState
        }
    };
    DeclareResource(*resourceProperties);

    BuildPipeline();
}

void DrawPass::OnBeforePass(unsigned int currentBufferingIndex)
{
    this->currentBufferingIndex = currentBufferingIndex;
    ReserveEnoughShaderResourceDescriptors(currentBufferingIndex);
    UpdateDrawItems(currentBufferingIndex);
    UpdateFrameResources(currentBufferingIndex);
}

void DrawPass::OnExecutePass(au::rhi::CommandRecorder* recorder)
{
    auto& resources = AcquireFrameResource(currentBufferingIndex);

    auto& color0 = resources.frameOutputs.colorOutputs.find("Color0");
    if (color0 == resources.frameOutputs.colorOutputs.end()) {
        GP_LOG_RET_E(TAG, "Not found color 0 output!");
    }
    auto& color1 = resources.frameOutputs.colorOutputs.find("Color1");
    if (color1 == resources.frameOutputs.colorOutputs.end()) {
        GP_LOG_RET_E(TAG, "Not found color 1 output!");
    }
    auto& colorDM = AcquireDescriptorManager(
        currentBufferingIndex, au::rhi::DescriptorType::ColorOutput);
    auto color0Descriptor = colorDM.AcquireDescriptor(0);
    auto color1Descriptor = colorDM.AcquireDescriptor(1);
    color0Descriptor->BuildDescriptor(color0->second->RawGpuInst(currentBufferingIndex), false);
    color1Descriptor->BuildDescriptor(color1->second->RawGpuInst(currentBufferingIndex), false);

    auto& depthStencil = resources.frameOutputs.depthStencilOutputs.find("DepthStencil");
    if (depthStencil == resources.frameOutputs.depthStencilOutputs.end()) {
        GP_LOG_RET_E(TAG, "Not found depth stencil output!");
    }
    auto& depthStencilDM = AcquireDescriptorManager(
        currentBufferingIndex, au::rhi::DescriptorType::DepthStencil);
    auto depthStencilDescriptor = depthStencilDM.AcquireDescriptor(0);
    depthStencilDescriptor->BuildDescriptor(
        depthStencil->second->RawGpuInst(currentBufferingIndex), false);

    auto& color0Properties = outputProperties->targets[OutputProperties::OutputSlot::C0];
    auto& color1Properties = outputProperties->targets[OutputProperties::OutputSlot::C1];
    auto& depthStencilProperties = outputProperties->targets[OutputProperties::OutputSlot::DS];

    unsigned int shaderResourceDescriptorIndex = 0;
    auto& shaderResourceDM = AcquireDescriptorManager(
        currentBufferingIndex, au::rhi::DescriptorType::ShaderResource);
    auto& sampledTexture = resources.frameResources.textures.find("SampledTexture");
    if (sampledTexture == resources.frameResources.textures.end()) {
        GP_LOG_RET_E(TAG, "Not found sampled texture!");
    }
    auto samTexDescriptor = shaderResourceDM.AcquireDescriptor(shaderResourceDescriptorIndex++);
    samTexDescriptor->BuildDescriptor(
        sampledTexture->second->RawGpuInst(currentBufferingIndex), false);

    auto& imageSamplerDM = AcquireDescriptorManager(
        currentBufferingIndex, au::rhi::DescriptorType::ImageSampler);
    auto& sampler = resources.frameResources.samplers.find("Sampler");
    if (sampler == resources.frameResources.samplers.end()) {
        GP_LOG_RET_E(TAG, "Not found image sampler!");
    }
    auto samplerDescriptor = imageSamplerDM.AcquireDescriptor(0);
    samplerDescriptor->BuildDescriptor(sampler->second->RawGpuInst());

    recorder->RcSetViewports({ { 0.0f, 0.0f,
        static_cast<float>(color0->second->GetWidth()),
        static_cast<float>(color0->second->GetHeight()) }});
    recorder->RcSetScissors({ { 0, 0,
    static_cast<long>(color0->second->GetWidth()),
    static_cast<long>(color0->second->GetHeight()) } });

    recorder->RcBarrier(color0->second->RawGpuInst(currentBufferingIndex),
        color0Properties.beforeState, color0Properties.currentState);
    recorder->RcBarrier(color1->second->RawGpuInst(currentBufferingIndex),
        color1Properties.beforeState, color1Properties.currentState);
    recorder->RcBarrier(depthStencil->second->RawGpuInst(currentBufferingIndex),
        depthStencilProperties.beforeState, depthStencilProperties.currentState);

    recorder->RcBeginPass(nullptr,
        {
            { color0Descriptor, color0Properties.beginAction, color0Properties.endAction },
            { color1Descriptor, color1Properties.beginAction, color1Properties.endAction }
        },
        {
            { depthStencilDescriptor,
              depthStencilProperties.beginAction, depthStencilProperties.endAction }
        });

    recorder->RcSetPipeline(AcquirePipelineState());
    recorder->RcSetDescriptorHeap({
        shaderResourceDM.AcquireDescriptorHeap(),
        imageSamplerDM.AcquireDescriptorHeap() });

    for (const auto& drawItem : resources.drawItems) {
        auto& vinColor = drawItem->itemResources.structuredBuffers.find("vinColor");
        if (vinColor == drawItem->itemResources.structuredBuffers.end()) {
            GP_LOG_W(TAG, "Not found draw item vin color buffer!");
            continue;
        }

        auto& mvpMat = drawItem->itemResources.constantBuffers.find("mvpMat");
        if (mvpMat == drawItem->itemResources.constantBuffers.end()) {
            GP_LOG_W(TAG, "Not found draw item mvp matrix data!");
            continue;
        }

        auto vinColorD = shaderResourceDM.AcquireDescriptor(shaderResourceDescriptorIndex++);
        vinColorD->BuildDescriptor(vinColor->second->RawGpuInst(0), false);

        auto mvpMatD = shaderResourceDM.AcquireDescriptor(shaderResourceDescriptorIndex++);
        mvpMatD->BuildDescriptor(mvpMat->second->RawGpuInst(currentBufferingIndex));

        recorder->RcSetGraphicsDescriptor(0, vinColorD);
        recorder->RcSetGraphicsDescriptor(1, mvpMatD);
        recorder->RcSetGraphicsDescriptor(2, samTexDescriptor);
        recorder->RcSetGraphicsDescriptor(3, samplerDescriptor);

        recorder->RcSetVertex(
            { drawItem->vertexBuffer->RawGpuInst(0) }, AcquireVertexAttributes());
        recorder->RcSetIndex(
            drawItem->indexBuffer->RawGpuInst(0), AcquireIndexAttribute());
        recorder->RcDraw(drawItem->indexBuffer->RawGpuInst(currentBufferingIndex));
    }

    recorder->RcEndPass();

    recorder->RcBarrier(color0->second->RawGpuInst(currentBufferingIndex),
        color0Properties.currentState, color0Properties.afterState);
    recorder->RcBarrier(color1->second->RawGpuInst(currentBufferingIndex),
        color1Properties.currentState, color1Properties.afterState);
    recorder->RcBarrier(depthStencil->second->RawGpuInst(currentBufferingIndex),
        depthStencilProperties.currentState, depthStencilProperties.afterState);
}

void DrawPass::OnAfterPass(unsigned int currentPassInFlowIndex)
{
    (void)currentPassInFlowIndex; // Nothing to do.
}

void DrawPass::OnEnablePass(bool enable)
{
    (void)enable; // Nothing to do.
}

class PresentPass : public au::gp::RasterizePass {
public:
    PresentPass(au::gp::Passflow& passflow);
    ~PresentPass() override;

protected:
    void OnPreparePass(au::rhi::Device* device) override;
    void OnBeforePass(unsigned int currentBufferingIndex) override;
    void OnExecutePass(au::rhi::CommandRecorder* recorder) override;
    void OnAfterPass(unsigned int currentPassInFlowIndex) override;
    void OnEnablePass(bool enable) override;

private:
    GP_LOG_TAG(PresentPass);

    unsigned int currentBufferingIndex = 0;
};

PresentPass::PresentPass(au::gp::Passflow& passflow) : RasterizePass(passflow)
{
}

PresentPass::~PresentPass()
{
}

void PresentPass::OnPreparePass(au::rhi::Device* device)
{
    (void)device; // Nothing to do.
}

void PresentPass::OnBeforePass(unsigned int currentBufferingIndex)
{
    this->currentBufferingIndex = currentBufferingIndex;
    // This Pass does not use DrawItem, so we do not need to call the UpdateDrawItems.
    UpdateFrameResources(currentBufferingIndex);
}

void PresentPass::OnExecutePass(au::rhi::CommandRecorder* recorder)
{
    auto& resources = AcquireFrameResource(currentBufferingIndex);

    auto& color = resources.frameOutputs.colorOutputs.find("Color");
    if (color == resources.frameOutputs.colorOutputs.end()) {
        GP_LOG_RET_E(TAG, "Not found color output!");
    }

    auto& presenter = resources.frameOutputs.displayPresentOutputs.find("Present");
    if (presenter == resources.frameOutputs.displayPresentOutputs.end()) {
        GP_LOG_RET_E(TAG, "Not found presenter output!");
    }

    recorder->RcBarrier(color->second->RawGpuInst(currentBufferingIndex),
        au::rhi::ResourceState::GENERAL_READ, au::rhi::ResourceState::COPY_SOURCE);
    recorder->RcBarrier(presenter->second->RawGpuInst(),
        au::rhi::ResourceState::PRESENT, au::rhi::ResourceState::COPY_DESTINATION);

    recorder->RcCopy(presenter->second->RawGpuInst(),
        color->second->RawGpuInst(currentBufferingIndex));

    recorder->RcBarrier(color->second->RawGpuInst(currentBufferingIndex),
        au::rhi::ResourceState::COPY_SOURCE, au::rhi::ResourceState::GENERAL_READ);
    recorder->RcBarrier(presenter->second->RawGpuInst(),
        au::rhi::ResourceState::COPY_DESTINATION, au::rhi::ResourceState::PRESENT);
}

void PresentPass::OnAfterPass(unsigned int currentPassInFlowIndex)
{
    auto& resources = AcquireFrameResource(currentBufferingIndex);
    auto& presenter = resources.frameOutputs.displayPresentOutputs.find("Present");
    if (presenter == resources.frameOutputs.displayPresentOutputs.end()) {
        GP_LOG_RET_E(TAG, "Not found presenter output!");
    }
    presenter->second->RawGpuInst()->Present();
}

void PresentPass::OnEnablePass(bool enable)
{
    (void)enable; // Nothing to do.
}

PassflowRP::~PassflowRP()
{
    passflow->CleanResource(cubeVertices);
    passflow->CleanResource(cubeIndices);

    passflow->CleanResource(cubeVerticesColors);

    passflow->CleanResource(cubeMVP);

    passflow->CleanResource(sampledTexture);
    passflow->CleanResource(textureSampler);

    passflow->CleanResource(displayOutput);
    passflow->CleanResource(depthStencilOutput);
    passflow->CleanResource(presentColorOutput);
    passflow->CleanResource(halfColorOutput);

    passflow.reset();
}

void PassflowRP::Setup()
{
    passflow = std::make_unique<au::gp::Passflow>("[Demo][Passflow]");

    drawPass = passflow->CreateOrGetPass<DrawPass>("[Demo][DrawPass]");
    passflow->EnablePass(passflow->AddPassToFlow(drawPass), true);
    presentPass = passflow->CreateOrGetPass<PresentPass>("[Demo][PresentPass]");
    passflow->EnablePass(passflow->AddPassToFlow(presentPass), true);

    cubeVertices = passflow->MakeResource<au::gp::VertexBuffer<DirectX::XMFLOAT3>>();
    cubeVertices->ConfigureAvoidInfight(false);
    cubeVertices->SetupVertexBuffer(static_cast<unsigned int>(CubeVertices.size()));
    cubeVertices->UpdateVertexBuffer(CubeVertices, 0);
    cubeVertices->UploadVertexBuffers();

    cubeIndices = passflow->MakeResource<au::gp::IndexBuffer<uint32_t>>();
    cubeIndices->ConfigureAvoidInfight(false);
    cubeIndices->SetupIndexBuffer(static_cast<unsigned int>(CubeIndices.size()));
    cubeIndices->UpdateIndexBuffer(CubeIndices, 0);
    cubeIndices->UploadIndexBuffers();

    cubeVerticesColors = passflow->MakeResource<au::gp::StructuredBuffer<DirectX::XMFLOAT4>>();
    cubeVerticesColors->ConfigureAvoidInfight(false);
    cubeVerticesColors->SetupStructuredBuffer(static_cast<unsigned int>(CubeColors.size()));
    cubeVerticesColors->UpdateStructuredBuffer(CubeColors, 0);
    cubeVerticesColors->UploadStructuredBuffers();

    cubeMVP = passflow->MakeResource<au::gp::ConstantBuffer<ObjectMVP>>();
    cubeMVP->SetupConstantBuffer();

    sampledTexture = passflow->MakeResource<au::gp::Texture2D>();
    sampledTexture->SetupTexture(au::rhi::BasicFormat::R32G32B32A32_FLOAT, 1, 1);
    auto colorFloat = reinterpret_cast<float*>(sampledTexture->AcquireTextureBuffer().data());
    colorFloat[0] = 0.5f;
    colorFloat[1] = 0.5f;
    colorFloat[2] = 0.5f;
    colorFloat[3] = 1.0f;
    sampledTexture->UploadTextureBuffers();

    textureSampler = passflow->MakeResource<au::gp::Sampler>();
    textureSampler->SetupSampler();
}

void PassflowRP::ExecuteOneFrame()
{
    AutomateRotate();
    UpdateData();
    Draw();
}

void PassflowRP::SizeChanged(void* window, unsigned int width, unsigned int height)
{
    if (displayOutput) {
        displayOutput->ResizeDisplay(width, height); // It will call WaitIdle.
    } else {
        displayOutput = passflow->MakeResource<au::gp::DisplayPresentOutput>();
        displayOutput->SetupDisplayPresentOutput(
            au::rhi::BasicFormat::R8G8B8A8_UNORM, width, height, window);
    }

    if (depthStencilOutput) {
        depthStencilOutput->ResizeDepthStencilOutput(width, height);
    } else {
        depthStencilOutput = passflow->MakeResource<au::gp::DepthStencilOutput>();
        depthStencilOutput->SetupDepthStencilOutput(
            au::rhi::BasicFormat::D24_UNORM_S8_UINT, width, height);
    }

    if (presentColorOutput) {
        presentColorOutput->ResizeColorOutput(width, height);
    } else {
        presentColorOutput = passflow->MakeResource<au::gp::ColorOutput>();
        presentColorOutput->SetupColorOutput(au::rhi::BasicFormat::R8G8B8A8_UNORM, width, height);
    }

    if (halfColorOutput) {
        halfColorOutput->ResizeColorOutput(width, height);
    } else {
        halfColorOutput = passflow->MakeResource<au::gp::ColorOutput>();
        halfColorOutput->SetupColorOutput(au::rhi::BasicFormat::R8G8B8A8_UNORM, width, height);
    }

    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void PassflowRP::AutomateRotate()
{
    theta += delta;

    // Convert Spherical to Cartesian coordinates.
    float x = radius * sinf(phi) * cosf(theta);
    float z = radius * sinf(phi) * sinf(theta);
    float y = radius * cosf(phi);

    // Build the model matrix.
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

    // Build the view matrix.
    DirectX::XMVECTOR position = DirectX::XMVectorSet(x, y, z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(position, target, up);

    // Build the projection matrix.
    DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
        0.25f * DirectX::XM_PI, aspectRatio, 0.1f, 100.0f);

    // Build the MVP matrix.
    DirectX::XMMATRIX mvp = world * view * projection;

    // Update the buffer with the latest MVP matrix.
    DirectX::XMStoreFloat4x4(&(cubeMVP->AcquireConstantBuffer().mvp), XMMatrixTranspose(mvp));
}

void PassflowRP::UpdateData()
{
    cubeMVP->UploadConstantBuffer(frameIndex);

    auto drawItem = std::make_shared<au::gp::DrawItem>();
    drawItem->indexBuffer = cubeIndices;
    drawItem->vertexBuffer = cubeVertices;
    drawItem->itemResources.structuredBuffers["vinColor"] = cubeVerticesColors;
    drawItem->itemResources.constantBuffers["mvpMat"] = cubeMVP;
    drawPass->AddDrawItem(drawItem);
    drawPass->ImportFrameResource("SampledTexture", sampledTexture);
    drawPass->ImportFrameResource("Sampler", textureSampler);
    drawPass->ImportFrameOutput("Color0", presentColorOutput);
    drawPass->ImportFrameOutput("Color1", halfColorOutput);
    drawPass->ImportFrameOutput("DepthStencil", depthStencilOutput);

    presentPass->ImportFrameOutput("Color", presentColorOutput);
    presentPass->ImportFrameOutput("Present", displayOutput);
}

void PassflowRP::Draw()
{
    frameIndex = passflow->ExecuteWorkflow();
}
