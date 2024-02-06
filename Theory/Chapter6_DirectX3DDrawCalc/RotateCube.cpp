#include "stdafx.h"

// ========================================================
//	물체 회전
// ========================================================

/*
	- 물체를 회전하기 위해서 회전 행렬을 곱해주면 된다.

	회전 행렬
		cos(angle),    sin(angle),  0.0f, 0.0f,
		-sin(angle),   cos(angle),  0.0f, 0.0f,
		0.0f,          0.0f,        1.0f, 0.0f,
		0.0f,          0.0f,        0.0f, 1.0f,

	- NDC 공간에서 화면 너비에 따라 기하구조의 비례도 결정되는 것을 방지하기 위해 [0][0]와 [1][0]에 "높이 / 너비"를 곱해준다.
	- row_major : HLSL은 열우선, DirectX는 행 우선이기 때문에 전치를 하거나 row_major를 해야 제대로 결과가 나온다.

	화면 비례에 따라 기하구조 변화 방지
		(높이 * 너비) * cos(angle),    sin(angle),  0.0f, 0.0f,
		(높이 * 너비) * -sin(angle),   cos(angle),  0.0f, 0.0f,
		0.0f,						  0.0f,        1.0f, 0.0f,
		0.0f,						  0.0f,        0.0f, 1.0f,
*/

// ========================================================
//	DirectX 수학
// ========================================================

/*
	- DirectX에서 행렬이나 4차원 좌표를 나타내는 구조를 가져오기 위해서는 DirectXMath.h를 가져오면 된다.

	DirectXMath 함수 설명
		- XMMatrixRotationZ : z에 대한 회전 행렬을 가져오기 위해서는 각도만 넣어주면 된다.
		- XMMatrixMultiply : 두 행렬을 곱하기 위한 함수.
		- XMMatrixScaling : 화면 비례에 따라 기하구조 비례 값을 바꾸는 함수
		- XMMatrixTranspose : 전치 행렬을 구해주는 함수
		- XMMatrixTranslation : 기하구조의 좌표를 입력한 매개변수에 따라 변화시켜는 함수
		- XMMatrixPerspectiveLH : Perspective으로 기하구조를 렌더링하도록 설정하는 함수

	- XMMATRIX는 SIMD로 인하여 *연산자를 이용해서 행렬 곱을 얻을 수 있다.
	
	Window 창 좌표계에서 NDC 공간 좌표 변환
		- NDC 공간 좌표 X : Window 창 좌표계 x / (창 너비 / 2) - 1
		- NDC 공간 좌표 Y : -(Window 창 좌표계 y / (창 너비 / 2) - 1)

	XMVECTOR
		- 4차원 벡터를 다루기 위한 데이터 형식
		
		관련 함수
			XMVector4Dot : 내적 함수
			XMVEctorGet_ : x, y, z, w 중 _ 성분을 가져오는 함수
			XMVector3Trasform : 3차원 Vector와 행렬을 곱해주는 함수
*/

// ========================================================
//	cbuffer와 SV_PrimitiveID
// ========================================================

/*
	- 상수 버퍼는 버텍스 셰이더와 픽셀 셰이더 단계에서 사용할 수 있다.

	- SV_PrimitiveID을 통해서 각 정점의 ID를 가져올 수 있다.
*/

// ========================================================
//	깊이 º 스텐실 버퍼
// ========================================================

/*
	- 깊이 º 스텐실 버퍼는 OM 단계에 처리한다.

	깊이 버퍼 (Depth Buffer)
		- 깊이 버퍼는 깊이를 화면상에 보여질 정점들의 깊이를 저장하기 위한 2차원 텍스쳐이다.
		- 픽셀 하나당 한개의 깊이 값을 가지고 있다.
		- 깊이 마스크(Depth Mask)를 통해서 일부의 깊이 값을 업데이트 하지 않도록 설정할 수 있다.

	스텐실 버퍼 (Stencil Buffer)
		- 후면 버퍼와 스텐실 버퍼와 같은 해상도를 가지는 버퍼
		- 특정부분을 렌더링하는 것을 방지하는데 사용
		- 사용 예로 거울 반사시 거울 뒷면 렌더 방지, 빛의 그림자 효과, 반사 등을 구현할 수 있다.

	깊이 스텐실 버퍼를 생성하기 위한 단계
		1. D3D11_DEPTH_STENCIL_DESC를 이용하여 깊이 스텐실 버퍼를 생성한다.
		2. D3D11_TEXTURE2D_DESC 타입으로 깊이 스텐실 버퍼가 그릴 2D 텍스처를 생성한다.
		3. D3D11_DEPTH_STENCIL_VIEW_DESC를 이용하여 깊이 스텐실 뷰를 생성한다.

		D3D11_DEPTH_STENCIL_DESC의 멤버 변수 설명
			- DepthWriteMask에 설정할 수 있는 값
				1. D3D11_DEPTH_WRITE_MASK_ALL : 모든 마스크에 깊이 값을 쓰도록 설정
				2. D3D11_DEPTH_WRITE_MASK_ZERO : 모든 마스크에 깊이 값을 쓰지 않도록 설정

			- DepthFunc에 설정할 수 있는 값
				1. D3D11_COMPARISON_LESS : 깊이 값보다 픽셀 깊이 값이 더 작을 경우 렌더링 함
				2. D3D11_COMPARISON_EQUAL : 깊이 값보다 픽셀 깊이 값이 같을 경우 렌더링 함
				3. D3D11_COMPARISON_GREATER : 깊이 값보다 픽셀 깊이 값이 더 클 경우 렌더링 함
				4. D3D11_COMPARISON_ALWAYS : 항상 픽셀 값을 렌더링하도록 설정
*/

namespace Sample
{
	// D3D11_DEPTH_STENCIL_DESC의 구조
	typedef struct D3D11_DEPTH_STENCIL_DESC
	{
		BOOL						DepthEnable;		// 깊이 값을 사용할 건지 여부
		D3D11_DEPTH_WRITE_MASK		DepthWriteMask;		// 어떤 깊이 마스크에 깊이 값을 쓸 여부
		D3D11_COMPARISON_FUNC		DepthFunc;			// 깊이 값이 어떨때 렌더링이 될 건지 여부
		BOOL						StencilEnable;		// 스텐실 값 사용 여부
		UINT8						StencilReadMask;	// 어떤 스텐실 마스크에 값을 읽을 건지 여부
		UINT8						StencilWriteMask;	// 어떤 스텐실 마스크에 값을 쓸 건지 여부
		D3D11_DEPTH_STENCILOP_DESC	FrontFace;			// 앞면에 대해서 어떤 결과를 수행할건지 수행 설명서
		D3D11_DEPTH_STENCILOP_DESC	BackFace;			// 뒷면에 대해서 어떤 결과를 수행할건지 수행 설명서 
	} 	D3D11_DEPTH_STENCIL_DESC;

	// D3D11_TEXTURE2D_DESC의 구조
	typedef struct D3D11_TEXTURE2D_DESC
	{
		UINT Width;						// 텍스처의 너비
		UINT Height;					// 텍스처의 높이
		UINT MipLevels;					// 원본 이미지의 다양항 버전을 저장할 개수
		UINT ArraySize;					// 여러개의 텍스처 배열이 있을 때 개수
		DXGI_FORMAT Format;				// 텍스처의 픽셀 형식
		DXGI_SAMPLE_DESC SampleDesc;	// 샘플링할 Desc
		D3D11_USAGE Usage;				// CPU 및 GPU 사용 여부
		UINT BindFlags;					// 텍스처 사용 여부
		UINT CPUAccessFlags;			// CPU 쓰기 읽기 여부
		UINT MiscFlags;					// 기타 설정
	} 	D3D11_TEXTURE2D_DESC;

	// D3D11_DEPTH_STENCIL_VIEW_DESC의 구조
	typedef struct D3D11_DEPTH_STENCIL_VIEW_DESC
	{
		DXGI_FORMAT					Format;			// 깊이 스텐실 뷰의 형식
		D3D11_DSV_DIMENSION			ViewDimension;	// 깊이 스텐실 텍스처 구조
		UINT						Flags;			// 기타 설정
			
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