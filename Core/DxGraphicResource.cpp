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
    return graphic.infoManager;
#else
    throw std::logic_error("Release������ Info Manager�� ������ �� ����");
#endif
}
