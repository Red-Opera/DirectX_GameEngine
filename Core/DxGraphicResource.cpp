#include "stdafx.h"
#include "DxGraphicResource.h"

#include <stdexcept>

ID3D11DeviceContext* DxGraphicResource::GetDeviceContext(DxGraphic& graphic) noexcept
{
    return graphic.deviceContext.Get();
}

ID3D11Device* DxGraphicResource::GetDevice(DxGraphic& graphic) noexcept
{
    return graphic.device.Get();
}

ExceptionInfo& DxGraphicResource::GetInfoManager(DxGraphic& graphic)
{
#ifndef NDEBUG
    // Debug 모드: 실제 infoManager 객체 반환
    return graphic.infoManager;
#else
    // Release 모드: 예외 발생으로 접근 차단
    throw std::logic_error("Release에서는 Info Manager를 접근할 수 없음");
#endif
}
