#pragma once

#include "BaseFile/Resource.h"

class DX3DApp
{
public:
	DX3DApp(HINSTANCE hInstance);
	virtual ~DX3DApp();

	HINSTANCE	AppInst() const;		// 인스턴스 핸들의 복사본을 반환하는 메소드
	HWND		MainWnd() const;		// 창의 핸들의 복사본을 반환하는 메소드
	float		AspectRatio() const;	// 후면 버퍼의 종횡비(Aspect Ratio), 즉 높이에 대한 너비의 비율을 반환함

	// 응용 프로그램을 시작할때 사용하는 코드
	int Run();

	// 프레임워크 메소드
	virtual bool	Init();														// 자원 할당, 장면 물체 초기화, 광원 설정 등 응용 프로그램 고유의 초기화 코드
	virtual void	OnResize();													// 클라이언트 영역의 크기라 변환시 처리되는 메소드
	virtual void	UpdateScene(float dt) = 0;									// 매 프레임마다 호출 (응용 프로그램 갱신)
	virtual void	DrawScene() = 0;											// 매 프레이마다 호출 (렌더링)
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);	// 응용 프로그램 주 창의 메세지 처리 기능

	// 마우스입력 처리를 위한 메소드
	virtual void OnMouseDown(WPARAM buttonState, int x, int y) { }
	virtual void OnMouseUp(WPARAM buttonState, int x, int y) { }
	virtual void OnMouseMove(WPARAM buttonState, int x, int y) { }

	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext; }

	bool GetUsageMSAA() const { return isMSAAUsage; }
	UINT GetMSAAQuality() const { return msaaQuality; }

	int GetClientWidth() const { return clientWidth; }
	int GetClientHeight() const { return clientHeight; }
	
	static float displayFPSSEC;

protected:
	bool InitMainWindow();			// 응용 프로그램의 주 창을 초기화
	bool InitDX3D();				// DirectX 3D를 초기화하는 메소드
	bool InitPipeLine();			// 렌더링 파이프라인을 초기화하는 메소드

	void CalculateFrameStates();	// 평균 초당 프레임 수(FPS)와 평균 프레임당 밀리초를 계산

	HINSTANCE	hInstance;			// 응용 프로그램 핸들
	HWND		hWnd;				// 주 창 핸들
	bool		isPause;			// 응용 프로그램이 일시정지 상태 여부
	
	bool		isMinimized;		// 응용 프로그램이 최소화된 상태 여부
	bool		isMaximized;		// 응용 프로그램이 최대화된 상태 여부
	bool		isResizing;			// 사용자가 크기 조정용 테두리를 끌고 있는지 여부

	bool		isMSAAUsage;		// 4X MSAA 사용 여부
	UINT		msaaQuality;		// 4X MSAA의 품질 수준

	// 응용 프로그램 실행 시간
	GameTimer	runningTime;

	ID3D11Device*			device;				// D3D11 장치
	ID3D11DeviceContext*	deviceContext;		// D3D11 Context
	IDXGISwapChain*			swapChain;			// 페이지 전환을 위한 교환 사슬
	ID3D11Texture2D*		depthStencilBuffer;	// 깊이 º 스텐실 버퍼를 위한 2차원 텍스처
	ID3D11RenderTargetView* renderTargetView;	// 렌더 대상용 2차원 텍스처
	ID3D11DepthStencilView* depthStencilView;	// 깊이 º 스텐실 뷰
	D3D11_VIEWPORT			viewport;			// 뷰포트

	std::wstring appTitle;			// 클라이언트 창 메뉴 이름

	D3D_DRIVER_TYPE driverType;		// 구동기 종류 (하드웨어 장치 또는 표준 장치)

	// 클라이언트 창 크기
	int clientWidth;				
	int clientHeight;
};

static DX3DApp* dx3dApp = 0;