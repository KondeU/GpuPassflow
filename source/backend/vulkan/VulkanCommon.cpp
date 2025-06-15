#include "VulkanCommon.h"

namespace au::backend {

std::string FormatResult(VkResult result)
{
    return std::to_string(string_VkResult(result));
}

}
