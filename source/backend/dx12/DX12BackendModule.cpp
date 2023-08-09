// DX12Context.h must be included before other self DX12 headers
// (to make sure that the DirectX header files is included first),
// otherwise the macro of `new` will cause a compilation error!
#include "DX12Context.h"
#include "DX12BaseObject.h"

extern "C" {

BackendApiExport au::rhi::BackendContext* CreateBackend()
{
    if (au::backend::DX12ObjectCounter::GetReference().GetObjectCount() > 0) {
        GP_LOG_E(au::backend::TAG, "Objects count is not zero when initialize!");
    }
    return new au::backend::DX12Context;
}

BackendApiExport void DestroyBackend(au::rhi::BackendContext* context)
{
    delete dynamic_cast<au::backend::DX12Context*>(context);
    if (au::backend::DX12ObjectCounter::GetReference().GetObjectCount() > 0) {
        GP_LOG_E(au::backend::TAG, "Objects count is not zero when finalize!");
    }
}

}
