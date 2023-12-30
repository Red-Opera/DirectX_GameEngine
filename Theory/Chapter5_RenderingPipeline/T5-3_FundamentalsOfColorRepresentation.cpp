#include "stdafx.h"

// ========================================================
//	색상 표현의 기초
// ========================================================

/*
	- 컴퓨터의 모니터는 빨간색, 녹색, 파란색 빛을 섞어서 방출한다.
	- RGB 값들로 색상을 표현하는 방식으로 색 편집할 수 있도록 Win32의 ChooseColor 함수를 지원한다.
*/

// ========================================================
//	색상 연산
// ========================================================

/*
	- 백터의 연산과 색상의 연산과 같다.

	색상 연산 중 합
		(r₁, g₁, b₁) + (r₂, g₂, b₂) = (r₁ + r₂, g₁ + g₂, b₁ + b₂)

	색상 연산 중 차
		(r₁, g₁, b₁) - (r₂, g₂, b₂) = (r₁ - r₂, g₁ - g₂, b₁ - b₂)

	색상 연산 중 곱셈
		- 백터의 곱셈과 색상 연산의 곱셈은 다르다.
		- 색상 연산 곱셈은 입사광선를 표면을 쐈을 때 반사되는 광선의 색을 구하는데 사용된다.

		(r₁, g₁, b₁) ⊗ (r₂, g₂, b₂) = (r₁r₂, g₁g₂, b₁b₂)
*/

// ========================================================
//	128비트 색상
// ========================================================

/*	
	 4번째 성분 알파
		- R, G, B 이외에 알파(A)이라는 벡터를 더 추가하는 경우도 있다.
		- 알파는 색상의 불투명도를 나타낸다.
	
	- 알파 성분을 추가한 색상 정보는 0 <= r, g, b, a <= 1인 4차원 벡터 (r, g, b, a)로 표현한다.
	- 각 성분은 32비트 float값으로 표현하여 하나의 색상을 128비트로 표현한다.
	- 4차원 색상은 XMVECTOR 형식을 이용하여 표현한다.
	- XMVECTOR를 이용하여 성분별 곱셈은 XNAMath에 있는 XMColorMoudlate를 이용한다.
*/

namespace Sample
{
	// XMColorMoudulate 함수 (반환 값 : 색상 연산 곱셈의 결과)
	XMVECTOR XMColorModulate(
		FXMVECTOR c1,	// 왼쪽 값
		FXMVECTOR c2	// 오른쪽 값
	);
}

// ========================================================
//	32비트 색상
// ========================================================

/*
	- 각 성분당 8비트를 할당하여 하나의 색상을 32비트로 표현할 수 있다.
	- 각 성분당 256가지의 세기를 표현할 수 있으며 0은 해당 성분이 없으며 255는 최대 세기이다.
	- 세 성분의 조합으로 256 x 256 x 256 x 256 = 16,777,216가지의 색을 나타낼 수 있다.
	- 32비트 색상을 저장하는 _XMCOLOR라는 구조체를 제공한다.

	색상 비트 변환 
		32비트 -> 128비트
			1. 각 성분을 255로 나눈다.

		128비트 -> 32비트
			1. 각 성분에 255를 곱한다.
			2. 가장 가까운 정수로 올린다.

		- 추가로 32비트 색상은 UINT에 채워넣는 형태기 때문에 추가적인 비트 연산이 필요한다.
		- XMCOLOR를 XMVECTOR로 바꾸기 위해서는 XMLoadColor라는 함수를 사용해야 한다.
		- XMVECTOR를 다시 XMCOLOR를 바꾸기 위해서는 XMStoreColor라는 함수를 사용하면 된다.
*/

namespace Sample
{
	// _XMCOLOR의 구조 (알파 성분들의 최상위 비트를 차지하고 청색 성분이 최하위 비트를 차지함)
	// aaaaaaaa rrrrrrrr gggggggg bbbbbbbb (즉 A8R8G8B8)
	typedef struct _XMCOLOR
	{
		union
		{
			struct
			{
				UINT b : 8; // 청색:		0/255에서 255/255까지
				UINT g : 8; // 녹색:		0/255에서 255/255까지
				UINT r : 8; // 적색:		0/255에서 255/255까지
				UINT a : 8; // 알파:		0/255에서 255/255까지
			};
			UINT c;
		};

#ifdef __cplusplus
		_XMCOLOR() {};
		_XMCOLOR(UINT Color) : c(Color) {};
		_XMCOLOR(FLOAT _r, FLOAT _g, FLOAT _b, FLOAT _a);
		_XMCOLOR(CONST FLOAT* pArray);

		operator UINT() { return c; }

		_XMCOLOR& operator= (CONST _XMCOLOR& Color);
		_XMCOLOR& operator= (CONST UINT Color);

#endif // __cplusplus
	}XMCOLOR;

	XMVECTOR XMLoadColor(CONST XMCOLOR* pSource);			// XMCOLOR -> XMVECTOR
	VOID XMStoreColor(XMCOLOR* pDestination, FXMVECTOR V);	// XMVECTOR -> XMCOLOR
}