#pragma once
#include "../Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(DxGraphic& graphic, const std::wstring& path);

	// Bindable��(��) ���� ��ӵ�
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

