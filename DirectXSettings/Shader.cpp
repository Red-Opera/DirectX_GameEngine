#include "stdafx.h"
#include "Shader.h"

Shader::Shader()
{
	DWORD shaderFlags = 0;

	// ���̴��� ����׽� ����ȭ�� ��
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;		// �����ϵ� ���̴� �ڵ�
	ID3D10Blob* errorMessage = 0;		// ���� �޼���

	// ���̴� �ڵ� ������
	HRESULT hr = D3DX11CompileFromFile(L"color.fx", 0,
		0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &errorMessage, 0);

	// ���� �޼����� ���� ���
	if (errorMessage != 0)
	{
		MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		ReleaseCOM(errorMessage);
	}

	// ������ Ȯ������ ������ ���� ���
	if (FAILED(hr))
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);

	ID3DX11Effect* shaderEffect;
	HR(D3DX11CreateEffectFromMemory(
		compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, dx3dApp->GetDevice(), &shaderEffect));

	ReleaseCOM(compiledShader);
}