#pragma once

#include <unordered_map>
#include <comdef.h> // DX12 COM.
#include "backend/BackendContext.h"

#define LogOutIfFailed(level, expression, success)    \
do {                                                  \
    HRESULT result = (expression);                    \
    success = SUCCEEDED(result);                      \
    if(!success) {                                    \
        au::gp::ErrorHandler::Logging(#level, TAG,    \
          "* LOC: %s, %d. EXP: %s. ERR: 0x%x, %s",    \
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

#define ReleaseCOM(com) \
do {                    \
    if (com) {          \
        com->Release(); \
        com = NULL;     \
    }                   \
} while(0)

namespace au::backend {

std::string FormatResult(HRESULT result);

template <typename Implement>
using InstanceContainer = std::unordered_map<Implement*, std::unique_ptr<Implement>>;

template <typename Interface, typename Implement, class ...Arguments>
Interface* CreateInstance(InstanceContainer<Implement>& container,
    typename Interface::Description description, typename Arguments& ...arguments)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "CreateInstance: Implement should inherit from Interface!");
    static_assert(std::is_base_of<DX12Object<Implement>, Implement>::value,
        "CreateInstance: Implement should inherit from DX12Object<Implement>!");
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
    static_assert(std::is_base_of<DX12Object<Implement>, Implement>::value,
        "DestroyInstance: Implement should inherit from DX12Object<Implement>!");
    return (container.erase(static_cast<Implement*>(instance)) > 0);
}

}
