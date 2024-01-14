#include "stdafx.h"

// ========================================================
//	ȿ�� �����ӿ�ũ
// ========================================================

/*
	- ���� ���� ȿ���� ��Ÿ���� ���ؼ� ���� ���̴�, �ȼ� ���̴�, ���� ���°� �ʿ��ϴ�.
	- ��, �ݼ�, �ִϸ��̼� �� �������� ȿ���� �����ϱ� ���� �����ӿ�ũ�� ȿ�� �����ӿ�ũ(Effect Framework)��� �Ѵ�.

	ȿ�� ���� .fx
		- ȿ���� .fx��� ȿ�� ���Ͽ� �ش�.
		- ȿ�� ������ ���̴� �ڵ�� ��� ���ۿ� �н�(pass)�� �����ϴ� �ϳ��� ���(Technique)�� �����ϴ� ���뵵 ����.
		- �н��� ���̴�, ����, ���� Ÿ�� ���� ���� �ϳ��� ȿ���� ��Ÿ���µ� ����Ѵ�.
		- ȿ�� ���Ͽ����� ���� ���յ� �����ϰ� �����ϴ� ���� �����ϴ�.

	ȿ�� ���� �ۼ���
		1. ȿ�� ���Ͽ��� �ϳ��� ����� �����ϱ� ���ؼ��� technique11 �ĺ��ڷ� �����Ѵ�.
			- technique11 �ĺ��ڷ� ���� �߰�ȣ ���� �ȿ� �ּ� �ϳ� �̻��� �н��� �־�� �Ѵ�.
			- �� �н����� ���� ������ �ٸ� ������� �������� �ϸ� Ư���� ������� �����Ͽ� ȿ���� �����Ѵ�.
			- ���� �н� ����� �������� �ؽ�ó�� ���ļ� ����� �� �ִ� ������ ������ ���ϱ����� �ٽ� �׸��� ������ ����� ��δ�.

		2. technique11 �ĺ��ڷ� ���� �߰�ȣ ���� �ȿ� pass �ĺ��ڷ� �����ϴ� ������ �߰�ȣ�� ���ؼ� �����Ѵ�.
			- ���� ������ ������������ �ܰ���� �� �� �ִ�.
			- �� �н� �ȿ��� ���ϱ����� ��� ó���ϰ� ���� �����Ұ��� ������ �� �ִ�.
*/

// ========================================================
//	���̴� �������ϱ�
// ========================================================

/*
	- .fx ���Ͽ� �ִ� ���̴��� �������ϱ� ���ؼ� DX3D11CompileFromFile ��ũ�θ� ����ϸ� �ȴ�.
	
	D3DX11CompileFromFile �Ű����� ����
		- pFunctionName : ȿ�� �����ӿ�ũ�� ����ϴ� ��� ȿ�� ������ ��� �н��� �������� ���ǵǾ� �����Ƿ� 0�� �Է��ص� �ȴ�.
		- pProfile : DirectX 3D 11�� fx_5_0�� �ۼ��Ͽ� 5.0������ ����Ѵ�.
		- Flags1�� ����� �� �ִ� ��
			1. D3D10_SHADER_DEBUG : ���̴��� ����� ���� ����
			2. D3D10_SHADER_SKIP_OPTIMIZATION : �����Ͻ� ����ȭ X

	- ������ ������ D3DX11CreateEffectFromMemery �Լ��� �̿��Ͽ� ID3D11Effect�� �����Ѵ�.
	- D3DX11Create__�� ���� �ڿ��� �����ϴ� �Լ��� ������ ���� ���� ������ �ʱ�ȭ���� ����Ѵ�.
*/

namespace Sample
{
	// D3DX11CompileFromFile ��ũ�� ����
	HRESULT D3DX11CompileFromFile(
		LPCTSTR pSrcFile,					// �������� fx���� �̸�
		CONST D3D10_SHADER_MACRO *pDefines,	// fx������ ��ó������ ���Ǹ� �����ϱ� ���� ��
		LPD3D10INCLUDE pInclude,			// fx���Ͽ��� �ٸ� �����̳� ����� �����Ҷ� �ʿ��� ���� �������̽�
		LPCSTR pFunctionName,				// ���̴� ���α׷��� �Լ� ������
		LPCSTR pProfile,					// ����� ���̴��� ����
		UINT Flags1,						// ������ ����� �÷���
		UINT Flags2,						// ������ ���� ��� ���� �÷���
		ID3DX11ThreadPump *pPump,			// ���̴��� �񵿱�� �������ϱ� ���� �ɼ�
		ID3D10Blob **ppShader,				// ������ �� ���̴� ������ ���� ����
		ID3D10Blob **ppErrorMsgs,			// ������ ������ ���� �޼����� ���� ����
		HRESULT *pHResult					// ���� �ڵ带 ���� ����
	);

	// D3DX11CreateEffectFromMemory �Լ� ����
	HRESULT D3DX11CreateEffectFromMemory(
		void* pData,				// ���̴� ����
		SIZE_T DataLength,			// ���̴� ������ ����Ʈ ũ��
		UINT FXFlags,				// D3DX11CompileFromFile�� Flags2�� ������ �� 
		ID3D11Device* pDevice,		// Device ����
		ID3DX11Effect **ppEffect	// ��ȯ�� ID3DX11Effect
	);
}

// ========================================================
//	C++ ���� ���α׷����� ȿ�� ��ü �ٷ��
// ========================================================

/*
	- C++ �ڵ�� Shader �ڵ尣�� ������ ��ȯ�� ��� ���� ������ ���ؼ� �� �� �ִ�.
	- ������ ������ ���� ID3DX11Effect�� �ִ� GetVariableByName�� ���ؼ� �������� �޾ƿ� ���� Set__�� �̿��Ͽ� ������.
	- GPU �޸𸮸� ȿ�������� �����ϱ� ���ؼ� Apply�� ȣ���� �߰��� �Ҷ� cbuffer�� �����Ͱ� ������ �ȴ�.
*/

namespace Sample
{
	/*
		cbuffer ExampleBuffer
		{
			int index;
			bool isEnter;
			float4 color;
			float4x4 worldViewProjection;
		}
	*/
	
	// ID3DX11Effect���� ��� ���� �������� ���
	void Sample4()
	{
		ID3DX11Effect* effect;

		ID3DX11EffectScalarVariable* efIndex;
		ID3DX11EffectScalarVariable* efisEnter;
		ID3DX11EffectVectorVariable* efColor;
		ID3DX11EffectMatrixVariable* efWVP;

		// ��� ���� �������� ���
		effect->GetVariableByName("index")->AsScalar();
		effect->GetVariableByName("isEnter")->AsScalar();
		effect->GetVariableByName("color")->AsVector();
		effect->GetVariableByName("worldViewProjection")->AsMatrix();

		// ��� ���� �������� ���
		XMMATRIX* worldViewProjection;
		XMVECTOR* color;

		efIndex->SetInt(1);
		efisEnter->SetBool(false);
		efColor->SetFloatVector((float*) &color);
		efWVP->SetMatrix((float*)&worldViewProjection);

		// fx���ϳ��� �ִ� ȿ�� ��� ��ü�� �������� ���
		ID3DX11EffectTechnique* tech;
		tech = effect->GetTechniqueByName("ColorTech");
	}
}

// ========================================================
//	ȿ���� �̿��� �׸���
// ========================================================

/*
	- ȿ���� ����� �̿��Ͽ� ���ϱ����� �׸��� ������ ������ �� pass�� ��ȸ�Ͽ� �� pass�� ������ �� �׸��� �ȴ�.
*/

namespace Sample
{
	void Sample5()
	{
		XMFLOAT4X4 worldPos;
		XMFLOAT4X4 viewPos;
		XMFLOAT4X4 projectionPos;

		XMMATRIX world = XMLoadFloat4x4(&worldPos);
		XMMATRIX view = XMLoadFloat4x4(&viewPos);
		XMMATRIX projection = XMLoadFloat4x4(&projectionPos);
		XMMATRIX worldViewProjection = world * view * projection;

		ID3DX11Effect* effect;
		ID3DX11EffectMatrixVariable* efWVP;

		effect->GetVariableByName("worldViewProjection")->AsMatrix();

		ID3DX11EffectTechnique* tech;
		tech = effect->GetTechniqueByName("ColorTech");

		// pass�� ��� ������ ���ϱ����� ȿ���� ������ desc
		D3DX11_TECHNIQUE_DESC techDesc;
		tech->GetDesc(&techDesc);

		for (UINT p = 0; p < techDesc.Passes; p++)
		{
			// �ε����� �ش��ϴ� pass�� ������ Apply���� ��� ���۸� �����ϰ� deviceContext�� ���ؼ� ������ ���������ο� ������
			tech->GetPassByIndex(p)->Apply(0, dx3dApp->GetDeviceContext());

			// pass�� ����� ���ϱ����� �׷���
			dx3dApp->GetDeviceContext()->DrawIndexed(36, 0, 0);
			
			// ���ο� ������ ���ϱ���
			XMMATRIX waveWVP;
			efWVP->SetMatrix(reinterpret_cast<float*>(&waveWVP));
			tech->GetPassByIndex(p)->Apply(0, dx3dApp->GetDeviceContext());

			// �ٸ� ȿ���� ���ϱ����� �׷���
			dx3dApp->GetDeviceContext()->DrawIndexed(36, 0, 0);
		}
	}
}

// ========================================================
//	'���̴� ������'�μ��� ȿ�� �����ӿ�ũ
// ========================================================

/*
	- �ϳ��� ȿ�� ���Ͼȿ� �������� ���(technique11�� ������ �߰�ȣ)�� ���� �� �ִ�.
	- �������� ����� ��� ���� �� �ϳ��� ������� �׷��� ī���� ǰ���� ���� �׸����� ǰ���� �������� �� �ִ�.

	������ ��� ��� ����

		float LowQualityPS(VertexOut pin) : SV_Target
		{
			// ���� ������ �۾��� ����
		}

		float MediumQualityPs(VertexOut pin) : SV_Target
		{
			// �߰� ������ �۾��� ����
		}

		float HighQualityPs(VertexOut pin) : SV_Target
		{
			// ��ǰ�� ������ �۾��� ����
		}

		technique11 ShadowLow
		{
			pass P0
			{
				VertexShder = compile vs_5_0 ColorVertexShader();
				PixelShader = compile ps_5_0 LowQualityPS();
			}
		}

		technique11 ShadowMedium
		{
			pass P0
			{
				VertexShder = compile vs_5_0 ColorVertexShader();
				PixelShader = compile ps_5_0 MediumQualityPS();
			}
		}

		technique11 ShadowHigh
		{
			pass P0
			{
				VertexShder = compile vs_5_0 ColorVertexShader();
				PixelShader = compile ps_5_0 HighQualityPS();
			}
		}

	- �� �ڵ�ó�� PS �Լ����� �������� �ڵ尡 ������ �� �ִ� �׷��� ������ ������ ���� �޾Ƽ� �ߺ��Ǵ� �κ��� if������ ó���ϸ� �ȴ�.

	����ȭ�� ���
		#define low 1
		#define medium 2
		#define high 3

		float PS(VertexOut pin, uniform int quality) : SV_Target
		{
			// ����� �۾� ó��

			if (quality == low)
			{
				// ��ǰ���� �� ó��
			}

			else if (quality == medium)
			{
				// �߰� ǰ���� �� ó��
			}

			else
			{
				// ��ǰ���϶� ó��
			}
		}

		technique11 ShadowLow
		{
			pass P0
			{
				VertexShder = compile vs_5_0 ColorVertexShader();
				PixelShader = compile ps_5_0 PS(1);
			}
		}

		technique11 ShadowMedium
		{
			pass P0
			{
				VertexShder = compile vs_5_0 ColorVertexShader();
				PixelShader = compile ps_5_0 PS(2);
			}
		}

		technique11 ShadowHigh
		{
			pass P0
			{
				VertexShder = compile vs_5_0 ColorVertexShader();
				PixelShader = compile ps_5_0 PS(3);
			}
		}

	- �� �ڵ忡�� �Է��� uniform�� ������ �Ű������� ������ �������� ������ �Ǵ� ����̴�.
	- �� �ڵ带 Ȱ���� �ٸ� ���÷� �ؽ��� ��� ����, ������ ���� ���� ���� �ٷ� �� �ִ�.
*/