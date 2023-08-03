#pragma once

#include "backend/BackendContext.h"

#define LogOutIfFailed(level, expression, success)                      \
do {                                                                    \
    HRESULT result = (expression);                                      \
    success = SUCCEEDED(result);                                        \
    if(!success) {                                                      \
        au::framework::log::Logger::GetReference().Logging(#level, TAG, \
          "LOCATION: %s, line %d. EXPRESSION: %s. ERROR: 0x%x, %s",     \
            __FILE__, __LINE__, #expression, result,                    \
            au::backend::FormatResult(result).c_str());                 \
    }                                                                   \
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

#define ReleaseCOM(com) \
do {                    \
    if (com) {          \
        com->Release(); \
        com = NULL;     \
    }                   \
} while(0)

namespace au::backend {

GP_LOG_TAG(DX12Backend);

std::string FormatResult(HRESULT result);

template <typename Interface, typename Implement, class ...Arguments>
Interface* CreateInstance(std::vector<std::unique_ptr<Implement>>& container,
    typename Interface::Description description, typename Arguments& ...arguments)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "CreateInstance: Implement should inherit from Interface!");
    static_assert(std::is_base_of<DX12Object<Implement>, Implement>::value,
        "CreateInstance: Implement should inherit from DX12Object<Implement>!");
    container.emplace_back(std::make_unique<Implement>(arguments...));
    container.back()->Setup(description);
    return container.back().get();
}

template <typename Interface, typename Implement>
bool DestroyInstance(std::vector<std::unique_ptr<Implement>>& container, Interface* instance)
{
    static_assert(std::is_base_of<Interface, Implement>::value,
        "CreateInstance: Implement should inherit from Interface!");
    static_assert(std::is_base_of<DX12Object<Implement>, Implement>::value,
        "CreateInstance: Implement should inherit from DX12Object<Implement>!");
    for (auto iter = container.begin(); iter != container.end(); iter++) {
        if (instance == iter->get()) {
            container.erase(iter);
            return true;
        }
    }
    return false;
}

}
