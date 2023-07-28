#include "framework/types/TString.hpp"
#include "DX12Device.h"

namespace au::backend {
DX12Device::DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi) : dxgi(dxgi)
{
    EnumAdapters();
}

DX12Device::~DX12Device()
{
    Shutdown();
}

void DX12Device::Setup(Description description)
{
    this->description = description;

    bool createHardwareDeviceSuccess = false;
    LogOutIfFailedI(D3D12CreateDevice(
        NULL, // use default adapter
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&device))
        , createHardwareDeviceSuccess);

    if (createHardwareDeviceSuccess) {
        AU_LOG_I(TAG, "Created DX12 hardware device with default adapter.");
    } else {
        AU_LOG_I(TAG, "Create DX12 hardware device failed, fallback to use soft warp device.");
        Microsoft::WRL::ComPtr<IDXGIAdapter> softWarpAdapter;
        LogIfFailedF(dxgi->EnumWarpAdapter(IID_PPV_ARGS(&softWarpAdapter)));
        LogIfFailedF(D3D12CreateDevice(softWarpAdapter.Get(),
            D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
    }

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    LogIfFailedF(device->CreateCommandQueue(
        &commandQueueDesc, IID_PPV_ARGS(&queues[CommandType::Graphics])));
    LogIfFailedF(device->CreateFence(fences[CommandType::Graphics].second,
        D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fences[CommandType::Graphics].first)));
}

void DX12Device::Shutdown()
{
    shaders.resize(0);
    swapchains.resize(0);
    commandRecorders.resize(0);
    inputVertices.resize(0);
    inputVertexAttributes.resize(0);
    inputIndices.resize(0);
    inputIndexAttributes.resize(0);
    resourceConstantBuffers.resize(0);
    resourceArrayBuffers.resize(0);
    resourceImages.resize(0);
    imageSamplers.resize(0);
    descriptorHeaps.resize(0);
    descriptorGroups.resize(0);
    pipelineLayouts.resize(0);
    pipelineStates.resize(0);
    allocators.clear();
    queues.clear();
    fences.clear();
    device.Reset();
}

Shader* DX12Device::CreateShader(Shader::Description description)
{
    return CreateInstance<Shader>(shaders, description);
}

bool DX12Device::DestroyShader(Shader* instance)
{
    return DestroyInstance(shaders, instance);
}

Swapchain* DX12Device::CreateSwapchain(Swapchain::Description description)
{
    return CreateInstance<Swapchain>(swapchains, description, *this);
}

bool DX12Device::DestroySwapchain(Swapchain* instance)
{
    return DestroyInstance(swapchains, instance);
}

CommandRecorder* DX12Device::CreateCommandRecorder(CommandRecorder::Description description)
{
    return CreateInstance<CommandRecorder>(commandRecorders, description, *this);
}

bool DX12Device::DestroyCommandRecorder(CommandRecorder* instance)
{
    return DestroyInstance(commandRecorders, instance);
}

InputVertex* DX12Device::CreateInputVertex(InputVertex::Description description)
{
    return CreateInstance<InputVertex>(inputVertices, description, *this);
}

bool DX12Device::DestroyInputVertex(InputVertex* instance)
{
    return DestroyInstance(inputVertices, instance);
}

InputVertexAttributes* DX12Device::CreateInputVertexAttributes()
{
    return CreateInstance<InputVertexAttributes>(inputVertexAttributes, {});
}

bool DX12Device::DestroyInputVertexAttributes(InputVertexAttributes* instance)
{
    return DestroyInstance(inputVertexAttributes, instance);
}

InputIndex* DX12Device::CreateInputIndex(InputIndex::Description description)
{
    return CreateInstance<InputIndex>(inputIndices, description, *this);
}

bool DX12Device::DestroyInputIndex(InputIndex* instance)
{
    return DestroyInstance(inputIndices, instance);
}

InputIndexAttribute* DX12Device::CreateInputIndexAttribute()
{
    return CreateInstance<InputIndexAttribute>(inputIndexAttributes, {});
}

bool DX12Device::DestroyInputIndexAttribute(InputIndexAttribute* instance)
{
    return DestroyInstance(inputIndexAttributes, instance);
}

ResourceBuffer* DX12Device::CreateResourceBuffer(ResourceBuffer::Description description)
{
    return CreateInstance<ResourceBuffer>(resourceConstantBuffers, description, *this);
}

bool DX12Device::DestroyResourceBuffer(ResourceBuffer* instance)
{
    return DestroyInstance(resourceConstantBuffers, instance);
}

ResourceBufferEx* DX12Device::CreateResourceBuffer(ResourceBufferEx::Description description)
{
    return CreateInstance<ResourceBufferEx>(resourceArrayBuffers, description, *this);
}

bool DX12Device::DestroyResourceBuffer(ResourceBufferEx* instance)
{
    return DestroyInstance(resourceArrayBuffers, instance);
}

ResourceImage* DX12Device::CreateResourceImage(ResourceImage::Description description)
{
    return CreateInstance<ResourceImage>(resourceImages, description, *this);
}

bool DX12Device::DestroyResourceImage(ResourceImage* instance)
{
    return DestroyInstance(resourceImages, instance);
}

ImageSampler* DX12Device::CreateImageSampler(ImageSampler::Description description)
{
    return CreateInstance<ImageSampler>(imageSamplers, description);
}

bool DX12Device::DestroyImageSampler(ImageSampler* instance)
{
    return DestroyInstance(imageSamplers, instance);
}

DescriptorHeap* DX12Device::CreateDescriptorHeap(DescriptorHeap::Description description)
{
    return CreateInstance<DescriptorHeap>(descriptorHeaps, description, *this);
}

bool DX12Device::DestroyDescriptorHeap(DescriptorHeap* instance)
{
    return DestroyInstance(descriptorHeaps, instance);
}

DescriptorGroup* DX12Device::CreateDescriptorGroup(DescriptorGroup::Description description)
{
    return CreateInstance<DescriptorGroup>(descriptorGroups, description, *this);
}

bool DX12Device::DestroyDescriptorGroup(DescriptorGroup* instance)
{
    return DestroyInstance(descriptorGroups, instance);
}

PipelineLayout* DX12Device::CreatePipelineLayout(PipelineLayout::Description description)
{
    return CreateInstance<PipelineLayout>(pipelineLayouts, description, *this);
}

bool DX12Device::DestroyPipelineLayout(PipelineLayout* instance)
{
    return DestroyInstance(pipelineLayouts, instance);
}

PipelineState* DX12Device::CreatePipelineState(PipelineState::Description description)
{
    return CreateInstance<PipelineState>(pipelineStates, description, *this);
}

bool DX12Device::DestroyPipelineState(PipelineState* instance)
{
    return DestroyInstance(pipelineStates, instance);
}

void DX12Device::WaitIdle()
{
    // TODO: Only a graphics command queue is used currently,
    //       so only one fence is used currently. :-)
    auto& queue = queues[CommandType::Graphics];
    auto& fence = fences[CommandType::Graphics].first;
    auto& currentFence = fences[CommandType::Graphics].second;

    // Advance the fence value to mark commands up to this fence point.
    currentFence++;

    // Add an instruction to the command queue to set a new fence point.
    // Because we are on the GPU timeline, the new fence point won't be
    // set until the GPU finishes processing all the commands prior to
    // this Signal().
    LogIfFailedF(queue->Signal(fence.Get(), currentFence));

    // Wait until the GPU has completed commands up to this fence point.
    if (fence->GetCompletedValue() < currentFence) {
        HANDLE eventHandle = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
        if (eventHandle != NULL) {
            // Fire event when GPU hits current fence.
            LogIfFailedF(fence->SetEventOnCompletion(currentFence, eventHandle));
            // Wait until the GPU hits current fence event is fired.
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        } else {
            AU_LOG_F(TAG, "Device wait idle failed, can not create event!");
        }
    }
}

void DX12Device::ReleaseCommandRecordersMemory(const std::string& commandContainer)
{
    auto iter = allocators.find(commandContainer);
    if (iter != allocators.end()) {
        LogIfFailedF(iter->second->Reset());
    } else {
        AU_LOG_W(TAG, "The command allocator was not reset because it(commandContainer=`%s`) "
            "was not found in the allocator.", commandContainer.c_str());
    }
}

Microsoft::WRL::ComPtr<IDXGIFactory4> DX12Device::DXGIFactory()
{
    return dxgi;
}

Microsoft::WRL::ComPtr<ID3D12Device> DX12Device::NativeDevice()
{
    return device;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue>
DX12Device::CommandQueue(CommandType type)
{
    // TODO: Only a CommandType::Graphics CommandQueue is used currently.
    return queues[CommandType::Graphics];
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator>
DX12Device::CommandAllocator(const std::string& name)
{
    auto& allocator = allocators[name];
    if (allocator == nullptr) {
        LogIfFailedF(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));
    }
    return allocator;
}

void DX12Device::EnumAdapters()
{
    AU_LOG_D(TAG, "Enum adapters...");
    std::vector<IDXGIAdapter*> adapters;

    AU_LOG_D(TAG, "Adapters:");
    IDXGIAdapter* adapter = nullptr;
    for (UINT i = 0; dxgi->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        AU_LOG_D(TAG, "* %d : %s", i, std::to_string(desc.Description).c_str());
        adapters.emplace_back(adapter);
    }

    AU_LOG_D(TAG, "Enum each adapter outputs...");
    for (size_t n = 0; n < adapters.size(); n++) {
        AU_LOG_D(TAG, "Adapter %d Outputs:", n);

        // Adaptor output: usually is a displayer(monitor).
        size_t outputsCount = 0;
        IDXGIOutput* output = nullptr;
        for (UINT i = 0; adapters[n]->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; i++) {
            DXGI_OUTPUT_DESC desc;
            output->GetDesc(&desc);
            AU_LOG_D(TAG, "* %d : %s", i, std::to_string(desc.DeviceName).c_str());

            AU_LOG_D(TAG, "  - OutputDisplayModes");
            {
                // Using default back buffer format to get display mode list.
                constexpr DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
                constexpr UINT flags = 0;

                UINT count = 0;
                output->GetDisplayModeList(format, flags, &count, NULL);

                std::vector<DXGI_MODE_DESC> modes(count);
                output->GetDisplayModeList(format, flags, &count, &modes[0]);

                for (const auto& mode : modes) {
                    UINT n = mode.RefreshRate.Numerator;
                    UINT d = mode.RefreshRate.Denominator;
                    std::string text =
                        "Width = " + std::to_string(mode.Width) + ", " +
                        "Height = " + std::to_string(mode.Height) + ", " +
                        "Refresh = " + std::to_string(n) + "/" + std::to_string(d) +
                        "=" + std::to_string(static_cast<float>(n) / static_cast<float>(d));
                    AU_LOG_D(TAG, "    %s", text.c_str());
                }
            }

            outputsCount++;
            ReleaseCOM(output);
        }

        if (outputsCount == 0) {
            AU_LOG_D(TAG, "Adapter %d has no output.", n);
        }
    }

    for (auto adapter : adapters) {
        ReleaseCOM(adapter);
    }
}
}
