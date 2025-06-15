#include "VulkanContext.h"
#include "VulkanObject.h"

extern "C" {

BackendApiExport au::rhi::BackendContext* CreateBackend()
{
    if (au::backend::VulkanObjectCounter::GetObjectCount() > 0) {
        GP_LOG_W(au::backend::TAG, "Objects count is not zero when initialize!");
    }
    auto context = new au::backend::VulkanContext;
    GP_LOG_I(au::backend::TAG, "Create Vulkan backend: `%p`.", context);
    return context;
}

BackendApiExport void DestroyBackend(au::rhi::BackendContext* context)
{
    delete dynamic_cast<au::backend::VulkanContext*>(context);
    GP_LOG_I(au::backend::TAG, "Destroy Vulkan backend: `%p`.", context);
    if (au::backend::VulkanObjectCounter::GetObjectCount() > 0) {
        GP_LOG_W(au::backend::TAG, "Objects count is not zero when finalize!");
    }
}

}
