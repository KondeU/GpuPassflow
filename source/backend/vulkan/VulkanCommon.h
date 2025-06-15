#pragma once

#include <unordered_map>
#include <vulkan/vk_enum_string_helper.h>
#include "backend/BackendContext.h"

#define LogOutIfFailed(level, expression, success)    \
do {                                                  \
    VkResult result = (expression);                   \
    success = (result == VK_SUCCESS);                 \
    if(!success) {                                    \
        au::gp::ErrorHandler::Logging(#level, TAG,    \
          "* LOC: %s, %d. EXP: %s. ERR: %d, %s",      \
          __FILE__, __LINE__, #expression, result,    \
          au::backend::FormatResult(result).c_str()); \
    }                                                 \
} while(0)

#define LogIfFailed(level, expression)          \
do {                                            \
    bool success = false;                       \
    LogOutIfFailed(level, expression, success); \
} while(0)

#define LogOutIfFailedD(expression, success) LogOutIfFailed(D, expression, success)
#define LogOutIfFailedI(expression, success) LogOutIfFailed(I, expression, success)
#define LogOutIfFailedW(expression, success) LogOutIfFailed(W, expression, success)
#define LogOutIfFailedE(expression, success) LogOutIfFailed(E, expression, success)
#define LogOutIfFailedF(expression, success) LogOutIfFailed(F, expression, success)

#define LogIfFailedD(expression) LogIfFailed(D, expression)
#define LogIfFailedI(expression) LogIfFailed(I, expression)
#define LogIfFailedW(expression) LogIfFailed(W, expression)
#define LogIfFailedE(expression) LogIfFailed(E, expression)
#define LogIfFailedF(expression) LogIfFailed(F, expression)

#define ReturnIfFailed(success)         if (!success) { return;        }
#define RetValIfFailed(success, retval) if (!success) { return retval; }

namespace au::backend {

GP_LOG_TAG(VulkanBackend);

std::string FormatResult(VkResult result);

template <typename Implement>
using InstanceContainer = std::unordered_map<Implement*, std::unique_ptr<Implement>>;

template <typename Interface, typename Implement, class ...Arguments>
Interface* CreateInstance(InstanceContainer<Implement>& container,
    typename Interface::Description description, typename Arguments& ...arguments)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "CreateInstance: Implement should inherit from Interface!");
    static_assert(std::is_base_of<VulkanObject<Implement>, Implement>::value,
        "CreateInstance: Implement should inherit from VulkanObject<Implement>!");
    auto implement = std::make_unique<Implement>(arguments...);
    if (!implement->Setup(description)) {
        return nullptr;
    }
    auto instance = implement.get();
    container[instance] = std::move(implement);
    return instance;
}

template <typename Interface, typename Implement>
bool DestroyInstance(InstanceContainer<Implement>& container, Interface* instance)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "DestroyInstance: Implement should inherit from Interface!");
    static_assert(std::is_base_of<VulkanObject<Implement>, Implement>::value,
        "DestroyInstance: Implement should inherit from VulkanObject<Implement>!");
    return (container.erase(static_cast<Implement*>(instance)) > 0);
}

}
