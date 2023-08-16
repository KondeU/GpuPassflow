#include "DX12Context.h"
#include "DX12BaseObject.h"

extern "C" {

BackendApiExport au::rhi::BackendContext* CreateBackend()
{
    if (au::backend::DX12ObjectCounter::GetObjectCount() > 0) {
        GP_LOG_W(au::backend::TAG, "Objects count is not zero when initialize!");
    }
    auto context = new au::backend::DX12Context;
    GP_LOG_I(au::backend::TAG, "Create DX12 backend: `%p`.", context);
    return context;
}

BackendApiExport void DestroyBackend(au::rhi::BackendContext* context)
{
    delete dynamic_cast<au::backend::DX12Context*>(context);
    GP_LOG_I(au::backend::TAG, "Destroy DX12 backend: `%p`.", context);
    if (au::backend::DX12ObjectCounter::GetObjectCount() > 0) {
        GP_LOG_W(au::backend::TAG, "Objects count is not zero when finalize!");
    }
}

}
