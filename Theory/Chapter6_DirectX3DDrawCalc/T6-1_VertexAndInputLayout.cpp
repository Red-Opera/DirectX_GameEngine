#include "stdafx.h"

// ========================================================
//	������ �Է� ��ġ
// ========================================================

/*
	- ������ ���� �� ��ġ ���� �̿ܿ� �ٸ� �ڷḦ ���� �� �ִµ� �̷��� ������ ��� ����ü�� ������ �Ѵ�.
	- ���� ����ü�� ����� ����ü�� �ɹ��� ���� �뵵�� ����ϴ��� �˷��ֱ� ���� I3D11InputLayout��� �Է� ��ġ(Input Layout) ��ü�� ������ �Ѵ�.
	- Input Layout�� ����� ���ؼ� ����ü �ɹ��� ��� ������ �Ǿ� �ִ��� �˷��ִ� D3D11_INPUT_ELEMENT_DESC�� �ۼ��ؾ� �Ѵ�.

	D3D11_INPUT_ELEMENT_DESC �ɹ�
		- SemanticName : ���� ���̴� �ܰ迡�� ����� �̸��� �� �ɹ��� ����
		- SemanticIndex : name1, name2�� ���� ���� �̸��� ����� �� �ֵ��� ���� �ε���
		- InputSlot : 0 ~ 15���� ���ڷ� �Էµ� �� ������, ���� ������ �ɹ��� �ٸ� ����ü�� ���� �� ����Ѵ�.
		- AlignedByteOffset : �տ� �ִ� �ɹ��� �뷮�� ��� ���� ���� ���� ������

	- D3D11_INPUT_ELEMENT_DESC�� �ۼ��ϰ� ���� ID3D11Device�� �ִ� CreateInputLayout�� �̿��Ͽ� InputLayout�� �����Ѵ�.

		pShaderBytecodeWithInputSignature
			- ���� ���̴����� �Է����� ���� �������� ������ ������ �Է� ����(Input Signature)��� �Ѵ�.
			- �� Input Signature�� ���� ���̴��� : �ڿ� ���� ������� �Է� ��ġ�� ���� �̸��� �����Ǿ�� �Ѵ�.
			- �Է� ��ġ ���� �� ��Ī �� ���� �ٸ� ���̴����� ������ �� �ִ�.

			���� ���� ����ü�� Input Signature(Vertex Shader �ڵ�)�� ����
				���� ���� ����ü
					struct Vertex
					{
						XMFLOAT3 Pos,
						XMFLOAT4 Color
					};
				
				Input Signature
					VertexOut VS(int3 pos : POSITION, float4 color : COLOR) { }

	- InputLaout�� ������ �� deviceContext�� �ִ� IASetInputLaout�� �̿��Ͽ� ������ ���������ο� ���´�
	- �ش� InputLayout�� �̿��Ͽ� ���������ϸ� �ٸ� InputLayout���� �ٲ𶧱��� ���������� �ش� InputLayout���� �׸���.
*/

namespace Sample
{
	// ���� ���� ����ü ����
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Texture
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 lt;
		XMFLOAT2 rb;
	};

	// D3D11_INPUT_ELEMENT_DESC�� ����
	typedef struct D3D11_INPUT_ELEMENT_DESC
	{
		LPCSTR						SemanticName;			// ���� ���̴� �ܰ迡 ���� �̸�
		UINT						SemanticIndex;			// SematicName�� �ε���
		DXGI_FORMAT					Format;					// �ش� �ɹ��� �ڷ� ����
		UINT						InputSlot;				// ���� ���� ����
		UINT						AlignedByteOffset;		// �ش� �ɹ��� ��ġ�ϴ� byte��ġ
		D3D11_INPUT_CLASSIFICATION	InputSlotClass;			// Vertex �� �Ǵ� �ν��Ͻ����� ����ϴ��� ����
		UINT						InstanceDataStepRate;	// �ν��Ͻ̿� ���϶� ������Ʈ�� �Ǵ��� ����
	};

	// CreateInputLayout �޼ҵ� ����
	class ID3D11Device
	{
		HRESULT CreateInputLayout(
			const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,	// ���� ����ü (D3D11_INPUT_ELEMENT_DESC)
			UINT NumElement,									// D3D11_INPUT_ELEMENT_DESC�� ���� ����
			const void* pShaderBytecodeWithInputSignature,		// ���� ���̴� ������ �ڵ带 ������ ������
			SIZE_T BytecodeLenth,								// ���� ���̴� ������ �ڵ��� ũ��
			ID3D11InputLayout** ppInputLayout					// InputLayout�� ��ȯ�� ����
		);
	};

	// D3D11_INPUT_ELEMENT_DESC �ۼ���
	// Vertex ����
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Texture ����
	D3D11_INPUT_ELEMENT_DESC textureDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// ID3D11Effect�� CreateInputLaout ����
	void Sample()
	{
		ID3DX11Effect* fx = nullptr;

		ID3DX11EffectTechnique* tech;
		ID3D11InputLayout* inputLaout;

		tech = fx->GetTechniqueByName("Tech");

		D3DX11_PASS_DESC passDesc;
		tech->GetPassByIndex(0)->GetDesc(&passDesc);

		//HR(device->CreateInputLaout(vertexDesc, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, *inputLaout);
	}
}