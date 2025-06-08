#include "VulkanSwapchain.h"

namespace au::backend {

VulkanSwapchain::VulkanSwapchain() {}
VulkanSwapchain::~VulkanSwapchain() { Shutdown(); }

bool VulkanSwapchain::Setup(Description description) {
    // TODO: Implement swapchain creation
    return true;
}

void VulkanSwapchain::Shutdown() {
    if (swapchain != VK_NULL_HANDLE && device != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }
    if (surface != VK_NULL_HANDLE && instance != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }
}

void VulkanSwapchain::Present() {
    // TODO: Implement present
}

rhi::ImageBuffer* VulkanSwapchain::GetBackBuffer() {
    // TODO: Implement get back buffer
    return nullptr;
}

uint32_t VulkanSwapchain::GetCurrentBackBufferIndex() {
    return currentImageIndex;
}

VkSwapchainKHR VulkanSwapchain::GetSwapchain() const { return swapchain; }
VkSurfaceKHR VulkanSwapchain::GetSurface() const { return surface; }

} 