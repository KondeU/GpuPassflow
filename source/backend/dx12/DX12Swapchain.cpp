#include "DX12Swapchain.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace au::backend {

DX12Swapchain::DX12Swapchain(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
    queue = internal.CommandQueue(rhi::CommandType::Graphics);
    descriptorSizeOfRtv = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeOfDsv = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

DX12Swapchain::~DX12Swapchain()
{
    Shutdown();
}

void DX12Swapchain::Setup(Description description)
{
    this->description = description;

    DXGI_SWAP_CHAIN_DESC swapchainDesc{};
    swapchainDesc.BufferDesc.Width = description.width;
    swapchainDesc.BufferDesc.Height = description.height;
    swapchainDesc.BufferDesc.RefreshRate.Numerator = description.refreshRate;
    swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapchainDesc.BufferDesc.Format = ConvertBasicFormat(description.colorFormat);
    swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.BufferCount = std::min(description.bufferCount, MaxBufferCountLimit);
    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;
    swapchainDesc.OutputWindow = (HWND)(description.window);
    swapchainDesc.Windowed = !description.fullScreen;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    // NB: Swapchain uses the queue to perform flush.
    LogIfFailedF(internal.DXGIFactory()->CreateSwapChain(queue.Get(), &swapchainDesc, &swapchain));

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = description.bufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    LogIfFailedF(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = description.bufferCount;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    LogIfFailedF(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));

    Resize(description.width, description.height);
}

void DX12Swapchain::Shutdown()
{
    internal.WaitIdle();
    swapchain.Reset();
    rtvHeap.Reset();
    dsvHeap.Reset();
    currentBufferIndex = 0;
    renderTargetHandlesInHeap.resize(0);
    depthStencilHandlesInHeap.resize(0);
    renderTargetBuffer.resize(0);
    depthStencilBuffer.resize(0);
}

void DX12Swapchain::Resize(unsigned int width, unsigned int height)
{
    // If call Resize when during rendering, call DX12Device::WaitIdle() first to make
    // sure that all command in the command queue have been executed, because Resize
    // will rebuild swaphain resources(render target or depth stencil resources/views).
    internal.WaitIdle();

    GP_LOG_I(TAG, "Swapchain resize: width * height = %d * %d", width, height);
    description.width = width;
    description.height = height;

    // Before resize swapchain, release all render target buffer references
    // which from the swapchain, otherwise resize will fail.
    renderTargetBuffer.resize(0);
    LogIfFailedF(swapchain->ResizeBuffers(description.bufferCount, width, height,
        ConvertBasicFormat(description.colorFormat), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    currentBufferIndex = 0;
    renderTargetHandlesInHeap.resize(description.bufferCount);
    depthStencilHandlesInHeap.resize(description.bufferCount);
    renderTargetBuffer.resize(description.bufferCount);
    depthStencilBuffer.resize(description.bufferCount);

    { // Get the render target buffer from swapchain and create the render target view.
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        for (unsigned int i = 0; i < description.bufferCount; i++) {
            LogIfFailedF(swapchain->GetBuffer(i, IID_PPV_ARGS(&renderTargetBuffer[i])));
            device->CreateRenderTargetView(renderTargetBuffer[i].Get(), NULL, rtvHeapHandle);
            renderTargetHandlesInHeap[i] = rtvHeapHandle;
            rtvHeapHandle.Offset(1, descriptorSizeOfRtv);
        }
    }

    { // Create the depth stencil buffer and view.
        D3D12_RESOURCE_DESC depthStencilDesc{};
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = description.width;
        depthStencilDesc.Height = description.height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1; // 1 mip levels
        depthStencilDesc.Format = ConvertBasicFormat(description.depthStencilFormat);
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE depthStencilClear{};
        depthStencilClear.Format = depthStencilDesc.Format;
        depthStencilClear.DepthStencil.Depth = 1.0f;
        depthStencilClear.DepthStencil.Stencil = 0;

        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
        depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
        depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Format = depthStencilDesc.Format;
        depthStencilViewDesc.Texture2D.MipSlice = 0; // mip level 0

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
        for (unsigned int i = 0; i < description.bufferCount; i++) {
            // Create the depth stencil buffer.
            LogIfFailedF(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
                &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClear,
                IID_PPV_ARGS(&depthStencilBuffer[i])));
            // Create descriptor(depth stencil view) for mip level 0 of each depth stencil buffer.
            device->CreateDepthStencilView(depthStencilBuffer[i].Get(),
                &depthStencilViewDesc, dsvHeapHandle);
            depthStencilHandlesInHeap[i] = dsvHeapHandle;
            dsvHeapHandle.Offset(1, descriptorSizeOfDsv);
        }
    }
}

void DX12Swapchain::Present()
{
    // Swap the back and front buffers.
    LogIfFailedF(swapchain->Present(0, 0));
    currentBufferIndex = (currentBufferIndex + 1) % description.bufferCount;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12Swapchain::CurrentRenderTargetBuffer()
{
    return renderTargetBuffer[currentBufferIndex];
}

Microsoft::WRL::ComPtr<ID3D12Resource> DX12Swapchain::CurrentDepthStencilBuffer()
{
    return depthStencilBuffer[currentBufferIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Swapchain::CurrentRenderTargetView()
{
    return renderTargetHandlesInHeap[currentBufferIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Swapchain::CurrentDepthStencilView()
{
    return depthStencilHandlesInHeap[currentBufferIndex];
}

D3D12_CLEAR_VALUE DX12Swapchain::RenderTargetClearValue() const
{
    return ConvertClearValue(description.colorFormat, description.colorClearValue);
}

D3D12_CLEAR_VALUE DX12Swapchain::DepthStencilClearValue() const
{
    return ConvertClearValue(description.depthStencilFormat, description.depthStencilClearValue);
}

D3D12_CLEAR_FLAGS DX12Swapchain::DepthStencilClearFlags() const
{
    return ConvertClearFlags(description.depthStencilFormat);
}

bool DX12Swapchain::IsSwapchainEnableDepthStencil() const
{
    return description.isEnabledDepthStencil;
}

}
