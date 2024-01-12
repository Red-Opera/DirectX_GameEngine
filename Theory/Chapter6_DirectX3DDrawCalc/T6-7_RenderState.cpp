#include "stdafx.h"

// ========================================================
//	렌더 상태
// ========================================================

/*
	- DirectX 3D는 하나의 상태 기계(State Machine)이기 때문에 입력 배치와 정점 버퍼, 색인 버퍼가 한번 묶이면 다른 것으로 묶기 전까지 유지된다.
	- 렌더 상태(Render State)라고 불리는 렌더링할 때 어떤 방식으로 렌더링을 수행할건지 정의하는 설정 집합을 이용하여 렌더링 파이프라인의 구성을 설정할 수 있다.

	렌더 상태 집합
		- ID3D11RaterizerState : 레스터회기 단계의 구성들을 설정할 수 있는 상태 집합
		- ID3D11BlendState : 혼합 연산을 구성을 설정하는 상태 집합
		- ID3D11DepthStencilState : 깊이 또는 스텐실 판정 구성을 설정하는 집합

		ID3D11RaterizerState 생성하는 방법
			1. D3D11_RASTERIZER_DESC 구조체를 작성한다.
			2. D3D11_RASTERIZER_DESC를 이용하여 ID3D11Device에 있는 CreateRaterizerState를 호출한다.
			3. ID3D11DeviceContext에 있는 RSSetState을 이용하여 RaterizerState를 렌더링 파이프라인에 등록한다.

		D3D11_RASTERIZER_DESC 맴버
			- FillMode : 오브젝트를 틀만 출력할건지 일반적으로 출력할건지 결정
				1. D3D11_FILL_WIREFRAME : 와이어 프레임만 렌더링할 경우
				2. D3D11_FILL_SOLID : 일반 고형체로 렌더링할 경우

			- CullMode : 앞면 또는 뒷면 렌더링 제거 여부
				1. D3D11_CULL_NONE : 앞면 뒷면 모두 렌더링할 경우
				2. D3D11_CULL_BACK : 뒷면을 렌더링하지 않을 경우
				3. D3D11_CULL_FRONT : 앞면을 렌더링하지 않을 경우

			- FrontCounterClockwise : 정점이 시계방향으로 할 경우 전면으로 간주하는 여부
				1. true : 시계방향으로 정점을 생성할 경우 앞면
				2. false : 반시계방향으로 정점을 생성할 경우 앞면

		- 서로 다양한 렌더 상태 집합은 프로그램의 초기화 시점에서 생성한 뒤 상황에 따라서 RaterizerState를 바꿔주기만 하면 된다.
		- 한번 입력한 RaterizerState은 다른 RaterizerState로 바꿀때까지 지속적으로 해당 오브젝트를 그릴때 적용된다.
		- RSSetState로 설정한 RaterizerState값을 다시 초기 상태로 돌리기 위해서 RaterizerState 변수 대신에 0을 입력해주면 된다.
*/

namespace Sample
{
	// D3D11_RASTERIZER_DESC의 구성
	typedef struct D3D11_RASTERIZER_DESC
	{
		D3D11_FILL_MODE FillMode;	// Default : D3D11_FILL_SOLID
		D3D11_CULL_MODE CullMode;	// Default : D3D11_CULL_BACK
		BOOL FrontCounterClockwise;	// Default : false
		INT DepthBias;				// Default : 0
		FLOAT DepthBiasClamp;		// Default : 0.0f
		FLOAT SlopeScaledDepthBias;	// Default : 0.0f
		BOOL DepthCilpEnable;		// Default : true
		BOOL ScissorEnable;			// Default : false
		BOOL MultisampleEnable;		// Default : false
		BOOL AntialiasedLineEnable;	// Default : false
	} D3D11_RASTERIZER_DESC;

	class ID3D11Device
	{
		// CreateRasterizerState 구성
		HRESULT CreateRasterizerState(
			const D3D11_RASTERIZER_DESC * pRasterizerDesc,	// D3D11_RASTERIZER_DESC 변수
			ID3D11RasterizerState **ppRasterizerState		// 반환될 RasterizerState 변수
		);
	};
	
	class ID3D11DeviceContext
	{
		// RasterizerState 구성
		void RSSetState(ID3D11RasterizerState * pRasterizerState);
	};
}