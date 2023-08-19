#include "DX12Common.h"

namespace au::backend {

std::string FormatResult(HRESULT result)
{
    return std::to_string(_com_error(result).ErrorMessage());
}

}
