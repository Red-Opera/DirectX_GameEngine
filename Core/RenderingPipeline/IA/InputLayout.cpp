#include "stdafx.h"
#include "InputLayout.h"
#include "Core/Exception/GraphicsException.h"
using namespace std;

InputLayout::InputLayout(DxGraphic& graphic, const vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* shaderCode)
{
	CREATEINFOMANAGER(graphic);

	GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateInputLayout(
		layout.data(),
		(UINT)layout.size(),
		shaderCode->GetBufferPointer(),
		shaderCode->GetBufferSize(),
		&inputLayout));
}

void InputLayout::PipeLineSet(DxGraphic& graphic) noexcept
{
	GetDeviceContext(graphic)->IASetInputLayout(inputLayout.Get());
}