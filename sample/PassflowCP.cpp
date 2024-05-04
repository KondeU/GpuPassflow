#include "PassflowCP.h"

namespace {

// Resources
constexpr auto SR = R"(
cbuffer inputProps : register(b0, space2)
{
    float width;    // output color attachment width (in pixels)
    float height;   // output color attachment height (in pixels)
    float time;     // shader playback time (in seconds)
    float delta;    // render time (in seconds)
    float frames;   // shader playback frame (frames count)
    float fps;      // shader frame rate (frame per second)
    float mouseX;   // mouse X coord (pixel)
    float mouseY;   // mouse Y coord (pixel)
    int mouseState; // mouse state, see FunctionDrivenBackgroundRenderSystem.h
    int reserved1;  // reserved, no used
    int reserved2;  // reserved, no used
    int reserved3;  // reserved, no used
};
Texture3D inputTex2Ds : register(t1, space2);
RWTexture2D<float4> outputColor : register(u2, space2);
SamplerState simpleSampler : register(s0, space3);
)";

// Processor
constexpr auto SP = R"(
[numthreads(8, 8, 1)]
void FDBR_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 pixel = dispatchThreadID.xy;
    if ((pixel.x < uint(width)) && (pixel.y < uint(height))) {
        // Custom defined: FDBR_Process(inout float4 fragColor, in uint2 fragCoord);
        FDBR_Process(outputColor[pixel], pixel);
    }
}
)";

// Default Process
constexpr auto DP = R"(
void FDBR_Process(inout float4 fragColor, in uint2 fragCoord)
{
    // Default nothing to do.
}
)";

class FunctionDrivenBackgroundRenderPass : public au::gp::ComputePass {
public:
    FunctionDrivenBackgroundRenderPass(au::gp::Passflow& passflow);
    ~FunctionDrivenBackgroundRenderPass() override;

    void SetFunctionShader(std::string shader);

protected:
    void OnPreparePass(au::rhi::Device* device) override;
    void OnBeforePass(unsigned int currentBufferingIndex) override;
    void OnExecutePass(au::rhi::CommandRecorder* recorder) override;
    void OnAfterPass(unsigned int currentPassInFlowIndex) override;
    void OnEnablePass(bool enable) override;

private:
    GP_LOG_TAG(FunctionDrivenBackgroundRenderPass);

    unsigned int currentBufferingIndex = 0;
    au::rhi::Device* backendDevice = nullptr;
    std::unique_ptr<au::gp::ProgramProperties> programProperties;
    std::unique_ptr<au::gp::ShaderResourceProperties> resourceProperties;
};

FunctionDrivenBackgroundRenderPass::FunctionDrivenBackgroundRenderPass(au::gp::Passflow& passflow)
    : ComputePass(passflow)
{
}

FunctionDrivenBackgroundRenderPass::~FunctionDrivenBackgroundRenderPass()
{
}

void FunctionDrivenBackgroundRenderPass::SetFunctionShader(std::string shader)
{
    if (!backendDevice) {
        GP_LOG_RET_E(TAG, "SetFunctionShader failed, backend device has not been stored!");
    }

    backendDevice->WaitIdle();

    CleanPipeline();
    InitializePipeline(backendDevice);

    programProperties = std::make_unique<au::gp::ProgramProperties>();
    programProperties->shaders[au::rhi::ShaderStage::Compute] = { SR + shader + SP, "FDBR_Main" };
    DeclareProgram(*programProperties);

    resourceProperties = std::make_unique<au::gp::ShaderResourceProperties>();
    resourceProperties->resources[au::gp::ShaderResourceProperties::ResourceSpace::PerScene] = {
        { // inputProps
            0,                                          // baseBindingPoint
            1,                                          // bindingPointCount
            au::rhi::ShaderStage::Compute,              // resourceVisibility
            au::rhi::DescriptorType::ConstantBuffer,    // resourceType
            au::rhi::ResourceState::GENERAL_READ,       // beforeState
            au::rhi::ResourceState::GENERAL_READ,       // currentState
            au::rhi::ResourceState::GENERAL_READ        // afterState
        },
        { // inputTex2Ds
            1,                                          // baseBindingPoint
            1,                                          // bindingPointCount
            au::rhi::ShaderStage::Compute,              // resourceVisibility
            au::rhi::DescriptorType::ReadOnlyTexture,   // resourceType
            au::rhi::ResourceState::GENERAL_READ,       // beforeState
            au::rhi::ResourceState::GENERAL_READ,       // currentState
            au::rhi::ResourceState::GENERAL_READ        // afterState
        },
        { // outputColor
            2,                                          // baseBindingPoint
            1,                                          // bindingPointCount
            au::rhi::ShaderStage::Compute,              // resourceVisibility
            au::rhi::DescriptorType::ReadWriteTexture,  // resourceType
            au::rhi::ResourceState::GENERAL_READ,       // beforeState
            au::rhi::ResourceState::GENERAL_READ_WRITE, // currentState
            au::rhi::ResourceState::GENERAL_READ        // afterState
        }
    };
    resourceProperties->resources[au::gp::ShaderResourceProperties::ResourceSpace::PerPass] = {
        { // simpleSampler
            0,                                          // baseBindingPoint
            1,                                          // bindingPointCount
            au::rhi::ShaderStage::Compute,              // resourceVisibility
            au::rhi::DescriptorType::ImageSampler,      // resourceType
            au::rhi::ResourceState::GENERAL_READ,       // beforeState
            au::rhi::ResourceState::GENERAL_READ,       // currentState
            au::rhi::ResourceState::GENERAL_READ        // afterState
        }
    };
    DeclareResource(*resourceProperties);

    if (!BuildPipeline()) {
        GP_LOG_W(TAG, "SetFunctionShader failed, custom shader error! Fallback to use default.");
        SetFunctionShader(DP);
    }
}

void FunctionDrivenBackgroundRenderPass::OnPreparePass(au::rhi::Device* device)
{
    backendDevice = device;
    SetFunctionShader(DP);
}

void FunctionDrivenBackgroundRenderPass::OnBeforePass(unsigned int currentBufferingIndex)
{
    this->currentBufferingIndex = currentBufferingIndex;
    ReserveEnoughDescriptors(currentBufferingIndex);
    UpdateFrameResources(currentBufferingIndex);
}

void FunctionDrivenBackgroundRenderPass::OnExecutePass(au::rhi::CommandRecorder* recorder)
{
    auto& frameResources = AcquireFrameResources(currentBufferingIndex);

    auto& shaderResourceDM = AcquireShaderResourceDescriptorManager(currentBufferingIndex);
    auto& imageSamplerDM = AcquireImageSamplerDescriptorManager(currentBufferingIndex);
    unsigned int shaderResourceDC = 0, imageSamplerDC = 0;

    auto& simpleSampler = frameResources.passResources.samplers.find("simpleSampler");
    if (simpleSampler == frameResources.passResources.samplers.end()) {
        GP_LOG_RET_E(TAG, "Not found simpleSampler in pass.");
    }
    auto simpleSamplerD = imageSamplerDM.AcquireDescriptor(imageSamplerDC++);
    simpleSamplerD->BuildDescriptor(simpleSampler->second->RawGpuInst());

    for (auto& [sceneKey, sceneResources] : frameResources.scenesResources) {
        auto& inputProps = sceneResources.sceneResources.constantBuffers.find("inputProps");
        if (inputProps == sceneResources.sceneResources.constantBuffers.end()) {
            GP_LOG_E(TAG, "Not found inputProps in [%s].", sceneKey);
            continue;
        }
        inputProps->second->UploadConstantBuffer(currentBufferingIndex);
        auto inputPropsD = shaderResourceDM.AcquireDescriptor(shaderResourceDC++);
        inputPropsD->BuildDescriptor(inputProps->second->RawGpuInst(currentBufferingIndex));

        auto& inputTex2Ds = sceneResources.sceneResources.textures.find("inputTex2Ds");
        if (inputTex2Ds == sceneResources.sceneResources.textures.end()) {
            GP_LOG_E(TAG, "Not found inputTex2Ds in [%s].", sceneKey);
            continue;
        }
        auto inputTex2DsD = shaderResourceDM.AcquireDescriptor(shaderResourceDC++);
        inputTex2DsD->BuildDescriptor(
            inputTex2Ds->second->RawGpuInst(currentBufferingIndex), false);

        auto& outputColor = sceneResources.sceneResources.textures.find("outputColor");
        if (outputColor == sceneResources.sceneResources.textures.end()) {
            GP_LOG_RET_E(TAG, "Not found outputColor in [%s].", sceneKey);
        }
        auto outputColorD = shaderResourceDM.AcquireDescriptor(shaderResourceDC++);
        outputColorD->BuildDescriptor(
            outputColor->second->RawGpuInst(currentBufferingIndex), true);

        auto& outputColorProperties = resourceProperties->
            resources[au::gp::ShaderResourceProperties::ResourceSpace::PerScene][2];

        recorder->RcBarrier(outputColor->second->RawGpuInst(currentBufferingIndex),
            outputColorProperties.beforeState, outputColorProperties.currentState);

        recorder->RcSetPipeline(AcquirePipelineState());
        recorder->RcSetDescriptorHeap({
            shaderResourceDM.AcquireDescriptorHeap(),
            imageSamplerDM.AcquireDescriptorHeap() });

        recorder->RcSetComputeDescriptor(0, inputPropsD);
        recorder->RcSetComputeDescriptor(1, inputTex2DsD);
        recorder->RcSetComputeDescriptor(2, outputColorD);
        recorder->RcSetComputeDescriptor(3, simpleSamplerD);

        unsigned int dispatchThreadX = (outputColor->second->GetWidth() + 7) / 8;
        unsigned int dispatchThreadY = (outputColor->second->GetHeight() + 7) / 8;
        recorder->RcDispatch(dispatchThreadX, dispatchThreadY, 1);

        recorder->RcBarrier(outputColor->second->RawGpuInst(currentBufferingIndex),
            outputColorProperties.currentState, outputColorProperties.afterState);

        #if defined (DEBUG) || defined (_DEBUG)
        if ((sceneResources.dispatchItems.size() != 1) ||
            (sceneResources.dispatchItems[0]->threadGroups[0] != dispatchThreadX) ||
            (sceneResources.dispatchItems[0]->threadGroups[1] != dispatchThreadY) ||
            (sceneResources.dispatchItems[0]->threadGroups[2] != 1)) {
            GP_LOG_D(TAG, "dispatchItems does not match the actual one, "
                "but it does not affect ours function driven background rendering.");
        }
        #endif
    }
}

void FunctionDrivenBackgroundRenderPass::OnAfterPass(unsigned int currentPassInFlowIndex)
{
    (void)currentPassInFlowIndex; // Nothing to do.
}

void FunctionDrivenBackgroundRenderPass::OnEnablePass(bool enable)
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
    auto& frameResources = AcquireFrameResources(currentBufferingIndex);

    for (auto& [sceneKey, sceneResources] : frameResources.scenesResources) {
        for (auto& [viewKey, viewResources] : sceneResources.viewsResources) {
            auto& color = viewResources.viewResources.textures.find("Color");
            if (color == viewResources.viewResources.textures.end()) {
                GP_LOG_E(TAG, "Not found color output in [%s,%s].", sceneKey, viewKey);
                continue;
            }

            auto& present = viewResources.viewOutputs.displayPresentOutputs.find("Present");
            if (present == viewResources.viewOutputs.displayPresentOutputs.end()) {
                GP_LOG_E(TAG, "Not found present output in [%s,%s].", sceneKey, viewKey);
                continue;
            }

            recorder->RcBarrier(color->second->RawGpuInst(currentBufferingIndex),
                au::rhi::ResourceState::GENERAL_READ, au::rhi::ResourceState::COPY_SOURCE);
            recorder->RcBarrier(present->second->RawGpuInst(),
                au::rhi::ResourceState::PRESENT, au::rhi::ResourceState::COPY_DESTINATION);

            recorder->RcCopy(present->second->RawGpuInst(),
                color->second->RawGpuInst(currentBufferingIndex));

            recorder->RcBarrier(color->second->RawGpuInst(currentBufferingIndex),
                au::rhi::ResourceState::COPY_SOURCE, au::rhi::ResourceState::GENERAL_READ);
            recorder->RcBarrier(present->second->RawGpuInst(),
                au::rhi::ResourceState::COPY_DESTINATION, au::rhi::ResourceState::PRESENT);
        }
    }
}

void PresentPass::OnAfterPass(unsigned int currentPassInFlowIndex)
{
    auto& frameResources = AcquireFrameResources(currentBufferingIndex);

    for (auto& [sceneKey, sceneResources] : frameResources.scenesResources) {
        for (auto& [viewKey, viewResources] : sceneResources.viewsResources) {
            auto& present = viewResources.viewOutputs.displayPresentOutputs.find("Present");
            if (present == viewResources.viewOutputs.displayPresentOutputs.end()) {
                GP_LOG_E(TAG, "Not found present output for presenting in [%s,%s].",
                    sceneKey, viewKey);
                continue;
            }
            present->second->RawGpuInst()->Present();
        }
    }
}

void PresentPass::OnEnablePass(bool enable)
{
    (void)enable; // Nothing to do.
}

}

PassflowCP::~PassflowCP()
{
    outputDisplay.reset();
    outputColor.reset();
    inputProperties.reset();
    input2DTexturesArray.reset();
    inputTextureSampler.reset();

    passflow.reset();
}

void PassflowCP::Setup()
{
    passflow = std::make_unique<au::gp::Passflow>("[Demo][Passflow]");

    auto fdbrPass = passflow->CreateOrGetPass<FunctionDrivenBackgroundRenderPass>(
        "[Demo][FunctionDrivenBackgroundRenderPass]");
    computePass = fdbrPass;
    passflow->EnablePass(passflow->AddPassToFlow(computePass), true);
    presentPass = passflow->CreateOrGetPass<PresentPass>("[Demo][PresentPass]");
    passflow->EnablePass(passflow->AddPassToFlow(presentPass), true);

    inputProperties = passflow->MakeResource<au::gp::ConstantBuffer<FDBRProperties>>();
    inputProperties->SetupConstantBuffer();
    inputProperties->AcquireConstantBuffer() = {};
    input2DTexturesArray = passflow->MakeResource<au::gp::Texture3D>();
    input2DTexturesArray->SetupTexture(au::rhi::BasicFormat::V32V32V32V32_FLOAT, 1, 1, 1);
    inputTextureSampler = passflow->MakeResource<au::gp::Sampler>();
    inputTextureSampler->SetupSampler();

    fdbrPass->SetFunctionShader(R"(
    // Refer from: https://www.shadertoy.com/view/XsXXDn
    float2 mod(float2 x, float y) { return (x - y * floor(x/y)); }
    void FDBR_Process(inout float4 fragColor, in uint2 fragCoord)
    {
        float t = time / 1000.;
        float2 r = float2(width, height);
        //==============================
        float3 c;
        float l, z=t;
        [unroll]for(int i=0; i<3; i++) {
            float2 uv, p=fragCoord.xy/r;
            uv=p;
            p-=.5;
            p.x*=r.x/r.y;
            z+=.07;
            l=length(p);
            uv+=p/l*(sin(z)+1.)*abs(sin(l*9.-z-z));
            c[i]=.01/length(mod(uv,1.)-.5);
        }
        fragColor=float4(c/l,t);
    })");
}

void PassflowCP::ExecuteOneFrame()
{
    auto current = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current - timestamp);
    auto delta = static_cast<float>(duration.count()) / 1000.0f; // ms
    timestamp = current;

    auto& properties = inputProperties->AcquireConstantBuffer();
    properties.width = static_cast<float>(outputColor->GetWidth());
    properties.height = static_cast<float>(outputColor->GetHeight());
    properties.time += delta;
    properties.delta = delta;
    properties.frames += 1.0f;
    properties.fps = properties.frames / (properties.time / 1000.0f);
    inputProperties->UploadConstantBuffer(frameIndex);

    computePass->MakeCurrent("defaultScene", "nonView");
    computePass->AddDispatchItem(dispatchItem);
    computePass->AddSceneResource("inputProps", inputProperties);
    computePass->AddSceneResource("inputTex2Ds", input2DTexturesArray);
    computePass->AddSceneResource("outputColor", outputColor);
    computePass->AddPassResource("simpleSampler", inputTextureSampler);

    presentPass->MakeCurrent("defaultScene", "onlyOneView");
    presentPass->AddViewResource("Color", outputColor);
    presentPass->AddOutput("Present", outputDisplay);

    frameIndex = passflow->ExecuteWorkflow();
}

void PassflowCP::SizeChanged(void* window, unsigned int width, unsigned int height)
{
    if (outputDisplay) {
        outputDisplay->ResizeDisplay(width, height); // It will call WaitIdle.
    } else {
        outputDisplay = passflow->MakeResource<au::gp::DisplayPresentOutput>();
        outputDisplay->SetupDisplayPresentOutput(
            au::rhi::BasicFormat::R8G8B8A8_UNORM, width, height, window);
    }

    if (outputColor) {
        outputColor->ResizeTexture(width, height);
    } else {
        outputColor = passflow->MakeResource<au::gp::Texture2D>();
        outputColor->ConfigureTextureUsage(au::rhi::ImageType::Color);
        outputColor->ConfigureTextureWritable(true);
        outputColor->SetupTexture(au::rhi::BasicFormat::R8G8B8A8_UNORM, width, height);
    }

    if (!dispatchItem) {
        dispatchItem = std::make_shared<au::gp::DispatchItem>();
    }
    dispatchItem->threadGroups[0] = (width + 7) / 8;
    dispatchItem->threadGroups[1] = (height + 7) / 8;
    dispatchItem->threadGroups[2] = 1;
}
