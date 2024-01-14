#include "stdafx.h"

// ========================================================
//	효과 프레임워크
// ========================================================

/*
	- 물과 같은 효과를 나타내기 위해서 정점 셰이더, 픽셀 셰이더, 렌더 상태가 필요하다.
	- 물, 금속, 애니메이션 등 여러가지 효과를 관리하기 위한 프레임워크를 효과 프레임워크(Effect Framework)라고 한다.

	효과 파일 .fx
		- 효과는 .fx라는 효과 파일에 준다.
		- 효과 파일은 셰이더 코드와 상수 버퍼와 패스(pass)를 포함하는 하나의 기법(Technique)을 정의하는 내용도 들어간다.
		- 패스는 셰이더, 상태, 렌더 타겟 등을 묶어 하나의 효과로 나타내는데 사용한다.
		- 효과 파일에서도 상태 집합도 생성하고 설정하는 것이 가능하다.

	효과 파일 작성법
		1. 효과 파일에서 하나의 기법을 정의하기 위해서는 technique11 식별자로 시작한다.
			- technique11 식별자로 만든 중괄호 내부 안에 최소 하나 이상의 패스가 있어야 한다.
			- 각 패스마다 기하 구조를 다른 방식으로 랜더링을 하며 특정한 방식으로 결합하여 효과를 구현한다.
			- 다중 패스 기법은 여러가지 텍스처를 겹쳐서 사용할 수 있는 장점이 있지만 기하구조를 다시 그리기 때문에 비용이 비싸다.

		2. technique11 식별자로 만든 중괄호 내부 안에 pass 식별자로 시작하는 섹션을 중괄호를 통해서 정의한다.
			- 보통 렌더링 파이프라인의 단계들이 들어갈 수 있다.
			- 이 패스 안에서 기하구조를 어떻게 처리하고 색을 지정할건지 지정할 수 있다.
*/

// ========================================================
//	셰이더 컴파일하기
// ========================================================

/*
	- .fx 파일에 있는 셰이더를 컴파일하기 위해서 DX3D11CompileFromFile 메크로를 사용하면 된다.
	
	D3DX11CompileFromFile 매개변수 설명
		- pFunctionName : 효과 프레임워크를 사용하는 경우 효과 파일의 기법 패스에 진입점이 정의되어 있으므로 0을 입력해도 된다.
		- pProfile : DirectX 3D 11은 fx_5_0로 작성하여 5.0버전을 사용한다.
		- Flags1에 사용할 수 있는 값
			1. D3D10_SHADER_DEBUG : 셰이더를 디버그 모드로 실행
			2. D3D10_SHADER_SKIP_OPTIMIZATION : 컴파일시 최적화 X

	- 컴파일 성공후 D3DX11CreateEffectFromMemery 함수를 이용하여 ID3D11Effect를 생성한다.
	- D3DX11Create__와 같이 자원을 생성하는 함수는 연산이 많이 들어가기 때문에 초기화에만 사용한다.
*/

namespace Sample
{
	// D3DX11CompileFromFile 메크로 구성
	HRESULT D3DX11CompileFromFile(
		LPCTSTR pSrcFile,					// 컴파일할 fx파일 이름
		CONST D3D10_SHADER_MACRO *pDefines,	// fx파일의 전처리기의 정의를 제공하기 위한 값
		LPD3D10INCLUDE pInclude,			// fx파일에서 다른 파일이나 헤더를 포함할때 필요한 정의 인터페이스
		LPCSTR pFunctionName,				// 세이더 프로그램의 함수 진입점
		LPCSTR pProfile,					// 사용할 셰이더의 버전
		UINT Flags1,						// 컴파일 방식의 플래그
		UINT Flags2,						// 컴파일 문법 경고 설정 플래그
		ID3DX11ThreadPump *pPump,			// 셰이더를 비동기로 컴파일하기 위한 옵션
		ID3D10Blob **ppShader,				// 컴파일 후 셰이더 파일을 받을 변수
		ID3D10Blob **ppErrorMsgs,			// 컴파일 오류시 오류 메세지를 담을 변수
		HRESULT *pHResult					// 오류 코드를 받을 변수
	);

	// D3DX11CreateEffectFromMemory 함수 구성
	HRESULT D3DX11CreateEffectFromMemory(
		void* pData,				// 셰이더 파일
		SIZE_T DataLength,			// 셰이더 파일의 바이트 크기
		UINT FXFlags,				// D3DX11CompileFromFile의 Flags2에 지정한 값 
		ID3D11Device* pDevice,		// Device 변수
		ID3DX11Effect **ppEffect	// 반환될 ID3DX11Effect
	);
}

// ========================================================
//	C++ 응용 프로그램에서 효과 객체 다루기
// ========================================================

/*
	- C++ 코드와 Shader 코드간의 데이터 교환은 상수 버퍼 변수를 통해서 할 수 있다.
	- 변수를 가져올 때는 ID3DX11Effect에 있는 GetVariableByName를 통해서 가져오고 받아올 때는 Set__를 이용하여 보낸다.
	- GPU 메모리를 효율적으로 갱신하기 위해서 Apply를 호출을 추가로 할때 cbuffer에 데이터가 갱신이 된다.
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
	
	// ID3DX11Effect에서 상수 버퍼 가져오는 방법
	void Sample4()
	{
		ID3DX11Effect* effect;

		ID3DX11EffectScalarVariable* efIndex;
		ID3DX11EffectScalarVariable* efisEnter;
		ID3DX11EffectVectorVariable* efColor;
		ID3DX11EffectMatrixVariable* efWVP;

		// 상수 버퍼 가져오는 방법
		effect->GetVariableByName("index")->AsScalar();
		effect->GetVariableByName("isEnter")->AsScalar();
		effect->GetVariableByName("color")->AsVector();
		effect->GetVariableByName("worldViewProjection")->AsMatrix();

		// 상수 버퍼 내보내는 방법
		XMMATRIX* worldViewProjection;
		XMVECTOR* color;

		efIndex->SetInt(1);
		efisEnter->SetBool(false);
		efColor->SetFloatVector((float*) &color);
		efWVP->SetMatrix((float*)&worldViewProjection);

		// fx파일내에 있는 효과 기법 객체를 가져오는 방법
		ID3DX11EffectTechnique* tech;
		tech = effect->GetTechniqueByName("ColorTech");
	}
}

// ========================================================
//	효과를 이용한 그리기
// ========================================================

/*
	- 효과의 기법을 이용하여 기하구조를 그리는 과정은 루프로 각 pass를 순회하여 각 pass를 적용한 후 그리면 된다.
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

		// pass를 모두 가져와 기하구조의 효과를 적용할 desc
		D3DX11_TECHNIQUE_DESC techDesc;
		tech->GetDesc(&techDesc);

		for (UINT p = 0; p < techDesc.Passes; p++)
		{
			// 인덱스에 해당하는 pass를 가져와 Apply에서 상수 버퍼를 갱신하고 deviceContext를 통해서 렌더링 파이프라인에 적용함
			tech->GetPassByIndex(p)->Apply(0, dx3dApp->GetDeviceContext());

			// pass로 적용된 기하구조가 그려짐
			dx3dApp->GetDeviceContext()->DrawIndexed(36, 0, 0);
			
			// 새로운 형태의 기하구조
			XMMATRIX waveWVP;
			efWVP->SetMatrix(reinterpret_cast<float*>(&waveWVP));
			tech->GetPassByIndex(p)->Apply(0, dx3dApp->GetDeviceContext());

			// 다른 효과로 기하구조가 그려짐
			dx3dApp->GetDeviceContext()->DrawIndexed(36, 0, 0);
		}
	}
}

// ========================================================
//	'셰이더 생성기'로서의 효과 프레임워크
// ========================================================

/*
	- 하나의 효과 파일안에 여러개의 기법(technique11를 수식한 중괄호)을 담을 수 있다.
	- 여러개의 기법을 담는 이유 중 하나로 사용자의 그래픽 카드의 품질에 따라 그림자의 품질을 결정해줄 수 있다.

	여려개 기법 사용 예시

		float LowQualityPS(VertexOut pin) : SV_Target
		{
			// 낮은 수준의 작업을 수행
		}

		float MediumQualityPs(VertexOut pin) : SV_Target
		{
			// 중간 수준의 작업을 수행
		}

		float HighQualityPs(VertexOut pin) : SV_Target
		{
			// 고품질 수준의 작업을 수행
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

	- 위 코드처럼 PS 함수들의 공통적인 코드가 존재할 수 있다 그래서 정수형 변수를 새로 받아서 중복되는 부분은 if문으로 처리하면 된다.

	최적화된 기법
		#define low 1
		#define medium 2
		#define high 3

		float PS(VertexOut pin, uniform int quality) : SV_Target
		{
			// 공통된 작업 처리

			if (quality == low)
			{
				// 저품질일 때 처리
			}

			else if (quality == medium)
			{
				// 중간 품질일 때 처리
			}

			else
			{
				// 고품질일때 처리
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

	- 위 코드에서 입력한 uniform을 수식한 매개변수는 컴파일 시점에서 결정이 되는 상수이다.
	- 위 코드를 활용한 다른 예시로 텍스쳐 사용 여부, 광원의 개수 설정 등을 다룰 수 있다.
*/