#pragma once
#include "../Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(DxGraphic& graphic, const std::wstring& path);
	
	ID3DBlob* GetShaderCode() const noexcept;

	// Bindable��(��) ���� ��ӵ�
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> shaderCode;				// �����ϵ� ���̴� �ڵ�
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;	// Vertex Shader
};

