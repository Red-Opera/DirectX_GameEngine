#pragma once

#include "../DxGraphic.h"

class Bindable
{
public:
	virtual void PipeLineSet(DxGraphic& graphic) noexcept = 0;
	virtual ~Bindable() = default;

protected:
	static ID3D11Device*		GetDevice(DxGraphic& graphic) noexcept;
	static ID3D11DeviceContext* GetDeviceContext(DxGraphic& graphic) noexcept;
	static ExceptionInfo&		GetInfoManager(DxGraphic& graphic) noexcept (_DEBUG);
};