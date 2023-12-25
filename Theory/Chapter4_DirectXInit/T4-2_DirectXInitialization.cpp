#include "stdafx.h"

// ========================================================
//	DirectX 3D의 초기화
// ========================================================

/*
	1. D3D11CreateDevice 함수를 이용하여 ID3D11Device 인터페이스와 ID3D11DeviceContext 인터페이스를 생성한다.
	2. ID3D11Device::CheckMultisampleQualityLevels를 이용하여 4X MASS 품질 수준 여부를 확인한다.
	3. DXGI_SWAP_CHAIN_DESE 구조체를 작성하여 교환 사슬의 특성을 지정한다.
	4. IDXGIFactory 인터페이스를 이용하여 IDXGISwapChain 인스턴스를 생성한다.
	5. 교환 사슬에 있는 후면 버퍼의 렌더 대상 뷰를 생성한다.
	6. 깊이º스텐실 버퍼와 버퍼와 연결되는 깊이º스텐실 뷰를 생성한다.
	7. 랜더 대상 뷰와 깊이º스텐실 뷰를 DirectX 3D가 사용할 수 있도록 렌더링 파이프라인의 출력 병합기 단계에 묶는다.
	8. 뷰포트를 설정한다.
*/

// ========================================================
//	장치와 장치 문맥 생성
// ========================================================

/*
	- DirectX 3D를 시작하기 위해서 ID3D11Device와 ID3D11DeviceContext를 생성해야 한다.

	ID3D11Device와 ID3D11DeviceContext
		1. ID3D11Device : DirectX 3D 11 장치
			- 하드웨어 기능 지원 확인 (Feature Level Check)
			- 자원을 할당하는데 사용

			- 
		2. ID3D11DeviceContext : DirectX 3D 11 장치의 문맥
			- 렌더링 결과를 출력되는 대상을 지정
			- GPU가 수행할 렌더링 명령을 지시

		- 그래픽 장치 하드웨어를 제어하기 위한 인터페이스
		- 하드웨어에게 GPU 메모리 자원 할당, 후면 버퍼 지우기, 여러 파이프라인 단계 묶기, 기하구조 그리기 등을 지시

	D3D11CreateDevice 매개변수 설명
		- DriverType에 지정할 수 있는 값
			1. D3D_DRIVER_TYPE_REFERENCE : Reference device를 생성하여 정확성을 목표로 하는 타입으로 디버깅이나 특수한 상황일 떄 사용 (하드웨어가 지원하지 않는 기능 시험, 구동기 버그 시험)
			2. D3D_DRIVER_TYPE_SOFTWARE : GPU를 사용하지 않고 소프트웨어 구동기(WARP(GPU를 사용하지 않고 CPU 사용) 구동기)로 흉내내는 타입으로 GPU 가속이 어려운 상황일 때 사용
			3. D3D_DRIVER_TYPE_WARP : D3D_DRIVER_TYPE_SOFTWARE보다 고퍼포먼스를 보여주는 WARP 구동기를 사용하기 위한 타입

		- Flags에 지정할 수 있는 플래그 값
			1. D3D11_CREATE_DEVICE_SINGLETHREADED : 멀티 쓰레드를 사용하지 않는 보장이 있을 때 사용하며 사용시 성능 향상 (ID3D11Device:CreateDeferredContext를 호출하면 실패)
			2. D3D11_CREATE_DEVICE_DEBUG : 런타임 디버깅 및 디바이스 상태 및 성능 정보를 확인하는데 도움을 줌 (VC++ 출력 창에 디버그 메시지로 표시)

*/

namespace Sample
{
	// D3D11CreateDevice 구성
	HRESULT D3D11CreateDevice(
		IDXGIAdapter* pAdapter,						// 디스플레이 어댑터(그래픽 카스 선택, 화면 모드 전환 역할)를 지정함 (0 또는 NULL로 설정할 경우 기본 어댑터를 사용)
		D3D_DRIVER_TYPE DriverType,					// 어떤 타입의 그래픽 드라이버를 사용할것인지 지정
		HMODULE Software,							// 소프트웨어 구동기를 사용할 때 지정하는 매개변수
		UINT Flags,									// 특정 동작을 추가할 때 사용하는 매개변수
		CONST D3D_FEATURE_LEVEL* pFeatureLevels,	// D3D_FEATURE_LEVEL 형식 원소들의 배열을 지정 (원소 순서대로 점검하는 순서, NULL 지정시 최고 기능 수준으로 설정)
		UINT FeatureLevels,							// D3D_FEATURE_LEVEL 형식 원소들의 배열의 길이
		UINT SDKVersion,							// D3D11_SDK_VERSION 지정
		ID3D11Device** ppDevice,					// D3D11CreateDevice가 설정한 Device를 반환함
		D3D_FEATURE_LEVEL* pFeatureLevel,			// pFeatureLevels 배열에서 맨 처음에 지원되는 수준
		ID3D11DeviceContext** pImmdiateContext		// D3D11CreateDevice가 설정한 DeviceContext를 반환함
	);
}

// ========================================================
//	4X MSAA 품질 수준 지원 점검
// ========================================================

/*
	- 장치를 생성한 후 이루어지는 과정
	- ID3D11Device로 만든 객체에 있는 CheckMultisampleQualityLevels를 이용한다.
	- CheckMultisampleQualityLevels에 마지막으로 입력된 UINT 값이 반환이 되면 0보다 큰지 확인한다.
	- 반환된 UINT 값이 0보다 클경우 4X MSAA를 지원을 한다는 뜻이다.
*/

// ========================================================
//	교환 사슬의 설정
// ========================================================

/*
	- 품질 수준 지원 점검 후 작성함
	- DXGI_SWAP_CHAIN_DESC 구조체를 작성해야 함

	DXGI_SWAP_CHAIN_DESC의 맴버 설명
		- SwapEffect에 설정할 수 있는 값
			1. DXGI_SWAP_EFFECT_DISCARD : 이전 프레임의 내용을 무시하고 새로운 프레임을 그림 (가장 효율적인 방법, 권장)
			2. DXGI_SWAP_EFFECT_SEQUENTIAL : 이전 프레임의 내용을 유지한 채로 새로운 프레임으로 교체

		- Flags에 설정할 수 있는 값
			1. DXGI_SWAP_CHAIN_FLAG_NONPREROTATED : 전면 버퍼의 내용을 화면으로 보낼 때 이미지 회전을 끔
			2. DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 전체화면에서 창모드 시 출력을 창에 맞춰 출력함

	DXGI_MODE_DESC의 맴버 설명
		- ScanlineOrdering : 화면이 그려지는 순서
			1. DXGI_MODE_SCALING_UNSPECIFIED : 화면이 그려지는 순서를 지정하지 않음 (하드웨어가 자동으로 순서를 지정)
			2. DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE : 화면을 위에서 아래로 순차적으로 출력함
			3. DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST : 인터레이스 스캔 (홀수번째 줄 먼저 출력후, 짝수번째 줄 출력)
			4. DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST : 인터레이스 스캔 (짝수번째 줄 먼저 출력후, 홀수번째 줄 출력)

		- Scaling : 화면에 출력할 때 결과물 이미지 크기
			1. DXGI_MODE_SCALING_UNSPECIFIED : 크기를 조절하거나 위치를 바꾸지 않음 (하드웨어가 자동으로 순서를 지정)
			2. DXGI_MODE_SCALING_CENTERED : 크기를 조절하지 않고 화면 중앙에 위치하도록 설정
			3. DXGI_MODE_SCALING_STRETCHED : 화면이 디스플레이의 화면에 맞춰짐
			4. DXGI_MODE_SCALING_ASPECT_RATIO_STRETCHED : 출력 결과물의 이미지의 비율에 맞춰 디스플레이 크기만큼 최대로 늘림
*/

namespace Sample
{
	// DXGI_SWAP_CHAIN_DESC 구조체
	typedef struct DXGI_SWAP_CHAIN_DESC
	{
		DXGI_MODE_DESC BufferDesc;					// 버퍼의 구체적인 속성을 설정된 구조체
		DXGI_SAMPLE_DESC SampleDesc;				// 다중표준화를 위한 표본의 개수와 품질 수준
		DXGI_USAGE BufferUsage;						// 버퍼의 사용 용도
		UINT BufferCount;							// 후면 버퍼의 개수
		HWND OutputWindow;							// 렌더링 결과를 출력할 창의 핸들
		BOOL Windowed;								// 창 모드 여부
		DXGI_SWAP_EFFECT SwapEffect;				// 후면 버퍼 교환시 효과
		UINT Flags;									// 추가적인 플래그
	} DXGI_SWAP_CHAIN_DESC;

	// DXGI_MODE_DESC
	typedef struct DXGI_MODE_DESC
	{
		UINT Width;									// 원하는 후면 버퍼 너비
		UINT Height;								// 원하는 후면 버퍼 높이
		DXGI_RATIONAL RefreshRate;					// 디스플레이 모드 갱신율
		DXGI_FORMAT Format;							// 후면 버퍼 픽셀 형식
		DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;	// 디스플레이 스캔라인 모드
		DXGI_MODE_SCALING Scaling;					// 디스플레이 비례모드
	} DXGI_MODE_DESC;
}

// ========================================================
//	교환 사슬의 생성
// ========================================================

/*
	- 교환 사슬을 설정했으면 생성을 한다.
	- IDXGIFactory의 IDXGIFactory::CreateSwapChain을 이용하여 교환 사슬(IDXGISwapChain)을 만든다.

	CreateSwapChain 이용법
		1. CreateSwapChain을 사용하기 위해 IDXGIFactory의 CreateDXGIFactory를 이용하여 인스턴스를 만든다.
		2. 그냥 사용하면 에러가 발생하기 때문에 Device 생성에 사용된 IDXGIFactory를 사용해야 한다.
		3. 그러한 과정은 COM 질의 과정을 걸쳐야 한다.
*/

namespace Sample
{
	class IDXGIFactory
	{
		HRESULT CreateSwapChain(
			IUnknown* pDevice,				// ID3D11Device를 가리키는 포인터
			DXGI_SWAP_CHAIN_DESC* pDesc,	// 교환 사슬 설명서
			IDXGISwapChain** ppSwapChain	// 설정된 값에 따라 반환될 변수
		);
	};
}

// ========================================================
//	렌더 대상 뷰의 생성
// ========================================================

/*
	- 후면 버퍼를 파이프라인에 연결하기 위해서는 후면 버퍼를 직접 연결하지 못하기 때문에 렌터 타겟 뷰를 생성한다.
	- 렌더 타겟 뷰는 출력 병합기(Output Merger) 단계에 묶어야 후면 버퍼에 그릴 수 있다.
*/

// ========================================================
//	깊이 º 스텐실 버퍼와 뷰 생성
// ========================================================

/*
	- 깊이 버퍼는 픽셀들의 깊이 정보를 담는 2차원 텍스처
	- D3D11_TEXTURE_DESC의 멤버 변수를 채운 후 ID3D11Device의 CreateTexutre2D 메소드를 호출하여 생성한다.

	D3D11_TEXTURE2D_DESC 멤버 변수 설명
		- Usage에 설정 가능한 값
			1. D3D11_USAGE_DEFAULT : 자원을 GPU가 읽고 쓸 수 있음 (CPU는 읽고 쓸 수가 없음)
			2. D3D11_USAGE_IMMUTABLE : 자원을 GPU가 읽기만 가능함 (CPU는 읽고 쓸 수가 없음, GPU는 쓸 수 없음)
			3. D3D11_USAGE_DYNAMIC : 자원을 GPU는 읽고 CPU는 쓸 때 사용 (자원을 빈번하게 갱신할 때 사용, 성능에 좋지 않음)
			4. D3D11_USAGE_STAGING : CPU 또는 GPU에서 읽기 쓰기 모두 가능 (CPU 자원을 GPU로 가져올 때 사용, 느린 연산으로 성능에 좋지 않음)

		- BindFlags에 설정할 수 있는 값
			1. D3D11_BIND_RENDER_TARGET : 텍스처를 렌더 대상으로 파이프 라인에 사용
			2. D3D11_BIND_SHADER_RESOURCE : 텍스처를 셰이더 자원으로 파이프 라인에 사용
			3. D3D11_BIND_DEPTH_STENCIL : 텍스처를 깊이 º 스텐실 버퍼 자원으로 파이프 라인에 사용

		- CPUAccessFlags : CPU가 자원을 접근하는 방식
			1. D3D11_CPU_ACCESS_WRITE : CPU가 자원에 자료를 기록
			2. D3D11_CPU_ACCESS_READ : CPU가 자원에 읽기 접근이 가능
*/

namespace Sample
{
	typedef struct D3D11_TEXTURE2D_DESC
	{
		UINT Width;						// 깊이 º 스텐실 버퍼의 너비
		UINT Height;					// 깊이 º 스텐실 버퍼의 높이
		UINT MipLevels;					// 밉맵 수준의 개수
		UINT ArraySize;					// 텍스처 배열의 개수 (깊이 º 스텐실 버퍼는 한개의 텍스처가 사용됨)
		DXGI_FORMAT Format;				// 텍셀의 형식으로 DXGI_FORMAT 열거형을 사용
		DXGI_SAMPLE_DESC SampleDesc;	// 다중 표본 개수와 품질 수준을 지정함
		D3D11_USAGE Usage;				// 텍스처의 용도 (텍스처를 어떤 용도로 렌더링 파이프 라인에 사용할 것인지 여부)
		UINT CPUAccessFlags;			// CPU가 자원을 접근하는 방식
		UINT MiscFlags;					// 기타 플래그
	} D3D11_TEXTURE2D_DESC;
}

// ========================================================
//	뷰들을 출력 병합기 단계에 묶기
// ========================================================

/*
	- 후면 버퍼와 깊이 º 스텐실 버퍼를 렌더링 파이프라인의 출력 병합기 단계에 묶어야 한다.
	- DeviceContext에 있는 OMSetRenderTargets 메소드를 이용하여 묶는다.
*/

// ========================================================
//	뷰포트 설정
// ========================================================

/*
	- 원하는 3차원 장면을 후면 버퍼의 일부를 직사각형 영역으로 설정
	- D3D11_VIEWPORT 구조체를 작성해야 한다.
	- VIEWPORT의 깊이 버퍼는 특수 효과를 사용하지 않을 때 최소는 0, 최대는 1로 설정한다.

	- D3D11_VIEWPORT를 완성 후 I3D11DeviceContext의 RSSetViewports를 이용하여 뷰포트를 설정한다.
*/

namespace Sample
{
	typedef struct D3D11_VIEWPORT
	{
		FLOAT TopLeftX;		// 뷰 포트 왼쪽 위 X 좌표
		FLOAT TopLeftY;		// 뷰 포트 왼쪽 위 Y 좌표
		FLOAT Width;		// 가로 크기
		FLOAT Height;		// 세로 크기
		FLOAT MinDepth;		// 최소 깊이 버퍼
		FLOAT MaxDepth;		// 최대 깊이 버퍼
	} D3D11_VIEWPORT;
}