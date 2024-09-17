#include "stdafx.h"
#include "Render.h"

namespace Graphic
{
    ID3D11DeviceContext* Render::GetDeviceContext(DxGraphic& graphic) noexcept { return graphic.GetDeviceContext(); }

    ID3D11Device* Render::GetDevice(DxGraphic& graphic) noexcept { return graphic.GetDevice(); }

    ExceptionInfo& Render::GetInfoManager(DxGraphic& graphic)
    {
#ifndef NDEBUG
        return graphic.GetInfoManager();
#else
        return logic_error("잘못된 그래픽 설정");
#endif // !NDEBUG

    }
}
