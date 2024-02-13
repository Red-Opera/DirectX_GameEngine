#pragma once
#include "../Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(DxGraphic& graphic, const std::wstring& path);
	
	ID3DBlob* GetShaderCode() const noexcept;

	// Bindable을(를) 통해 상속됨
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> shaderCode;				// 컴파일된 셰이더 코드
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;	// Vertex Shader
};

