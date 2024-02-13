#pragma once
#include "../Bindable.h"

class SamplerState : public Bindable
{
public:
	SamplerState(DxGraphic& graphic);

	// Bindable��(��) ���� ��ӵ�
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};