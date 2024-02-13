#pragma once
#include "../Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(
		DxGraphic& graphic, 
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		ID3DBlob* shaderCode);

	// Bindable��(��) ���� ��ӵ�
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

