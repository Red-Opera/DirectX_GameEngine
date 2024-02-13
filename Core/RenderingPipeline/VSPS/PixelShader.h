#pragma once
#include "../Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(DxGraphic& graphic, const std::wstring& path);

	// Bindable을(를) 통해 상속됨
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

