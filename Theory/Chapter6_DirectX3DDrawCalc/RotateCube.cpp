#include "stdafx.h"

// ========================================================
//	��ü ȸ��
// ========================================================

/*
	- ��ü�� ȸ���ϱ� ���ؼ� ȸ�� ����� �����ָ� �ȴ�.

	ȸ�� ���
		cos(angle),    sin(angle),  0.0f, 0.0f,
		-sin(angle),   cos(angle),  0.0f, 0.0f,
		0.0f,          0.0f,        1.0f, 0.0f,
		0.0f,          0.0f,        0.0f, 1.0f,

	- NDC �������� ȭ�� �ʺ� ���� ���ϱ����� ��ʵ� �����Ǵ� ���� �����ϱ� ���� [0][0]�� [1][0]�� "���� / �ʺ�"�� �����ش�.
	- row_major : HLSL�� ���켱, DirectX�� �� �켱�̱� ������ ��ġ�� �ϰų� row_major�� �ؾ� ����� ����� ���´�.

	ȭ�� ��ʿ� ���� ���ϱ��� ��ȭ ����
		(���� * �ʺ�) * cos(angle),    sin(angle),  0.0f, 0.0f,
		(���� * �ʺ�) * -sin(angle),   cos(angle),  0.0f, 0.0f,
		0.0f,						  0.0f,        1.0f, 0.0f,
		0.0f,						  0.0f,        0.0f, 1.0f,
*/

// ========================================================
//	DirectX ����
// ========================================================

/*
	- DirectX���� ����̳� 4���� ��ǥ�� ��Ÿ���� ������ �������� ���ؼ��� DirectXMath.h�� �������� �ȴ�.

	DirectXMath �Լ� ����
		- XMMatrixRotationZ : z�� ���� ȸ�� ����� �������� ���ؼ��� ������ �־��ָ� �ȴ�.
		- XMMatrixMultiply : �� ����� ���ϱ� ���� �Լ�.
		- XMMatrixScaling : ȭ�� ��ʿ� ���� ���ϱ��� ��� ���� �ٲٴ� �Լ�
		- XMMatrixTranspose : ��ġ ����� �����ִ� �Լ�
		- XMMatrixTranslation : ���ϱ����� ��ǥ�� �Է��� �Ű������� ���� ��ȭ���Ѵ� �Լ�
		- XMMatrixPerspectiveLH : Perspective���� ���ϱ����� �������ϵ��� �����ϴ� �Լ�

	- XMMATRIX�� SIMD�� ���Ͽ� *�����ڸ� �̿��ؼ� ��� ���� ���� �� �ִ�.
	
	Window â ��ǥ�迡�� NDC ���� ��ǥ ��ȯ
		- NDC ���� ��ǥ X : Window â ��ǥ�� x / (â �ʺ� / 2) - 1
		- NDC ���� ��ǥ Y : -(Window â ��ǥ�� y / (â �ʺ� / 2) - 1)

	XMVECTOR
		- 4���� ���͸� �ٷ�� ���� ������ ����
		
		���� �Լ�
			XMVector4Dot : ���� �Լ�
			XMVEctorGet_ : x, y, z, w �� _ ������ �������� �Լ�
			XMVector3Trasform : 3���� Vector�� ����� �����ִ� �Լ�
*/

// ========================================================
//	cbuffer�� SV_PrimitiveID
// ========================================================

/*
	- ��� ���۴� ���ؽ� ���̴��� �ȼ� ���̴� �ܰ迡�� ����� �� �ִ�.

	- SV_PrimitiveID�� ���ؼ� �� ������ ID�� ������ �� �ִ�.
*/

// ========================================================
//	���� �� ���ٽ� ����
// ========================================================

/*
	- ���� �� ���ٽ� ���۴� OM �ܰ迡 ó���Ѵ�.

	���� ���� (Depth Buffer)
		- ���� ���۴� ���̸� ȭ��� ������ �������� ���̸� �����ϱ� ���� 2���� �ؽ����̴�.
		- �ȼ� �ϳ��� �Ѱ��� ���� ���� ������ �ִ�.
		- ���� ����ũ(Depth Mask)�� ���ؼ� �Ϻ��� ���� ���� ������Ʈ ���� �ʵ��� ������ �� �ִ�.

	���ٽ� ���� (Stencil Buffer)
		- �ĸ� ���ۿ� ���ٽ� ���ۿ� ���� �ػ󵵸� ������ ����
		- Ư���κ��� �������ϴ� ���� �����ϴµ� ���
		- ��� ���� �ſ� �ݻ�� �ſ� �޸� ���� ����, ���� �׸��� ȿ��, �ݻ� ���� ������ �� �ִ�.

	���� ���ٽ� ���۸� �����ϱ� ���� �ܰ�
		1. D3D11_DEPTH_STENCIL_DESC�� �̿��Ͽ� ���� ���ٽ� ���۸� �����Ѵ�.
		2. D3D11_TEXTURE2D_DESC Ÿ������ ���� ���ٽ� ���۰� �׸� 2D �ؽ�ó�� �����Ѵ�.
		3. D3D11_DEPTH_STENCIL_VIEW_DESC�� �̿��Ͽ� ���� ���ٽ� �並 �����Ѵ�.

		D3D11_DEPTH_STENCIL_DESC�� ��� ���� ����
			- DepthWriteMask�� ������ �� �ִ� ��
				1. D3D11_DEPTH_WRITE_MASK_ALL : ��� ����ũ�� ���� ���� ������ ����
				2. D3D11_DEPTH_WRITE_MASK_ZERO : ��� ����ũ�� ���� ���� ���� �ʵ��� ����

			- DepthFunc�� ������ �� �ִ� ��
				1. D3D11_COMPARISON_LESS : ���� ������ �ȼ� ���� ���� �� ���� ��� ������ ��
				2. D3D11_COMPARISON_EQUAL : ���� ������ �ȼ� ���� ���� ���� ��� ������ ��
				3. D3D11_COMPARISON_GREATER : ���� ������ �ȼ� ���� ���� �� Ŭ ��� ������ ��
				4. D3D11_COMPARISON_ALWAYS : �׻� �ȼ� ���� �������ϵ��� ����
*/

namespace Sample
{
	// D3D11_DEPTH_STENCIL_DESC�� ����
	typedef struct D3D11_DEPTH_STENCIL_DESC
	{
		BOOL						DepthEnable;		// ���� ���� ����� ���� ����
		D3D11_DEPTH_WRITE_MASK		DepthWriteMask;		// � ���� ����ũ�� ���� ���� �� ����
		D3D11_COMPARISON_FUNC		DepthFunc;			// ���� ���� ��� �������� �� ���� ����
		BOOL						StencilEnable;		// ���ٽ� �� ��� ����
		UINT8						StencilReadMask;	// � ���ٽ� ����ũ�� ���� ���� ���� ����
		UINT8						StencilWriteMask;	// � ���ٽ� ����ũ�� ���� �� ���� ����
		D3D11_DEPTH_STENCILOP_DESC	FrontFace;			// �ո鿡 ���ؼ� � ����� �����Ұ��� ���� ����
		D3D11_DEPTH_STENCILOP_DESC	BackFace;			// �޸鿡 ���ؼ� � ����� �����Ұ��� ���� ���� 
	} 	D3D11_DEPTH_STENCIL_DESC;

	// D3D11_TEXTURE2D_DESC�� ����
	typedef struct D3D11_TEXTURE2D_DESC
	{
		UINT Width;						// �ؽ�ó�� �ʺ�
		UINT Height;					// �ؽ�ó�� ����
		UINT MipLevels;					// ���� �̹����� �پ��� ������ ������ ����
		UINT ArraySize;					// �������� �ؽ�ó �迭�� ���� �� ����
		DXGI_FORMAT Format;				// �ؽ�ó�� �ȼ� ����
		DXGI_SAMPLE_DESC SampleDesc;	// ���ø��� Desc
		D3D11_USAGE Usage;				// CPU �� GPU ��� ����
		UINT BindFlags;					// �ؽ�ó ��� ����
		UINT CPUAccessFlags;			// CPU ���� �б� ����
		UINT MiscFlags;					// ��Ÿ ����
	} 	D3D11_TEXTURE2D_DESC;

	// D3D11_DEPTH_STENCIL_VIEW_DESC�� ����
	typedef struct D3D11_DEPTH_STENCIL_VIEW_DESC
	{
		DXGI_FORMAT					Format;			// ���� ���ٽ� ���� ����
		D3D11_DSV_DIMENSION			ViewDimension;	// ���� ���ٽ� �ؽ�ó ����
		UINT						Flags;			// ��Ÿ ����
			
		union
		{
			D3D11_TEX1D_DSV			Texture1D;
			D3D11_TEX1D_ARRAY_DSV	Texture1DArray;
			D3D11_TEX2D_DSV			Texture2D;
			D3D11_TEX2D_ARRAY_DSV	Texture2DArray;
			D3D11_TEX2DMS_DSV		Texture2DMS;
			D3D11_TEX2DMS_ARRAY_DSV Texture2DMSArray;
		};
	} 	D3D11_DEPTH_STENCIL_VIEW_DESC;
}