// TString must be included first.
#include "framework/types/TString.hpp"
#include "DX12Common.h"
#include <comdef.h>

namespace au::backend {

std::string FormatResult(HRESULT result)
{
    return std::to_string(_com_error(result).ErrorMessage());
}

}
