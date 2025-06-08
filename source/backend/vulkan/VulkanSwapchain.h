#pragma once

#include "VulkanBackendHeaders.h"
#include "VulkanObject.h"

namespace au::backend {

class VulkanSwapchain : public rhi::Swapchain
    , VulkanObject<VulkanSwapchain> {
public:
    explicit VulkanSwapchain();
    ~VulkanSwapchain() override;

    bool Setup(Description description);
    void Shutdown();

    void Present() override;
    rhi::ImageBuffer* GetBackBuffer() override;
    uint32_t GetCurrentBackBufferIndex() override;

    VkSwapchainKHR GetSwapchain() const;
    VkSurfaceKHR GetSurface() const;

private:
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    uint32_t currentImageIndex = 0;
};

} 