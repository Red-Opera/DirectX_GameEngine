#pragma once
#include "../Bindable.h"

class SamplerState : public Bindable
{
public:
	SamplerState(DxGraphic& graphic);

	// Bindable을(를) 통해 상속됨
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};