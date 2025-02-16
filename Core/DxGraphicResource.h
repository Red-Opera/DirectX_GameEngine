#pragma once

#include "DxGraphic.h"

class DxGraphic;

class DxGraphicResource
{
protected:
	static ID3D11DeviceContext* GetDeviceContext(DxGraphic& graphic) noexcept;
	static ID3D11Device* GetDevice(DxGraphic& graphic) noexcept;
	static ExceptionInfo& GetInfoManager(DxGraphic& graphic);
};