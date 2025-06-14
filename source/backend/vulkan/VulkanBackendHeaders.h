#pragma once

#if defined(WIN32) || defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xlib.h>
#elif defined(__APPLE__)
#define VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>
#else
#include <vulkan/vulkan.h>
#endif
#include "backend/BackendContext.h"
