#include "stdafx.h"
#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetDeviceContext(DxGraphic& graphic) noexcept
{
    return graphic.GetDeviceContext();
}

ID3D11Device* Bindable::GetDevice(DxGraphic& graphic) noexcept
{
    return graphic.GetDevice();
}

ExceptionInfo& Bindable::GetInfoManager(DxGraphic& graphic) noexcept (_DEBUG)
{
#ifndef NDEBUG
    return graphic.GetInfoManager();
#else
    return logic_error("잘못된 그래픽 설정");
#endif // !NDEBUG

}
