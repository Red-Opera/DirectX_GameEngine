#pragma once

#include "BaseFile/Resource.h"

class DX3DApp
{
public:
	DX3DApp(HINSTANCE hInstance);
	virtual ~DX3DApp();

	HINSTANCE	AppInst() const;		// �ν��Ͻ� �ڵ��� ���纻�� ��ȯ�ϴ� �޼ҵ�
	HWND		MainWnd() const;		// â�� �ڵ��� ���纻�� ��ȯ�ϴ� �޼ҵ�
	float		AspectRatio() const;	// �ĸ� ������ ��Ⱦ��(Aspect Ratio), �� ���̿� ���� �ʺ��� ������ ��ȯ��

	// ���� ���α׷��� �����Ҷ� ����ϴ� �ڵ�
	int Run();

	// �����ӿ�ũ �޼ҵ�
	virtual bool	Init();														// �ڿ� �Ҵ�, ��� ��ü �ʱ�ȭ, ���� ���� �� ���� ���α׷� ������ �ʱ�ȭ �ڵ�
	virtual void	OnResize();													// Ŭ���̾�Ʈ ������ ũ��� ��ȯ�� ó���Ǵ� �޼ҵ�
	virtual void	UpdateScene(float dt) = 0;									// �� �����Ӹ��� ȣ�� (���� ���α׷� ����)
	virtual void	DrawScene() = 0;											// �� �����̸��� ȣ�� (������)
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);	// ���� ���α׷� �� â�� �޼��� ó�� ���

	// ���콺�Է� ó���� ���� �޼ҵ�
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
	bool InitMainWindow();			// ���� ���α׷��� �� â�� �ʱ�ȭ
	bool InitDX3D();				// DirectX 3D�� �ʱ�ȭ�ϴ� �޼ҵ�
	bool InitPipeLine();			// ������ ������������ �ʱ�ȭ�ϴ� �޼ҵ�

	void CalculateFrameStates();	// ��� �ʴ� ������ ��(FPS)�� ��� �����Ӵ� �и��ʸ� ���

	HINSTANCE	hInstance;			// ���� ���α׷� �ڵ�
	HWND		hWnd;				// �� â �ڵ�
	bool		isPause;			// ���� ���α׷��� �Ͻ����� ���� ����
	
	bool		isMinimized;		// ���� ���α׷��� �ּ�ȭ�� ���� ����
	bool		isMaximized;		// ���� ���α׷��� �ִ�ȭ�� ���� ����
	bool		isResizing;			// ����ڰ� ũ�� ������ �׵θ��� ���� �ִ��� ����

	bool		isMSAAUsage;		// 4X MSAA ��� ����
	UINT		msaaQuality;		// 4X MSAA�� ǰ�� ����

	// ���� ���α׷� ���� �ð�
	GameTimer	runningTime;

	ID3D11Device*			device;				// D3D11 ��ġ
	ID3D11DeviceContext*	deviceContext;		// D3D11 Context
	IDXGISwapChain*			swapChain;			// ������ ��ȯ�� ���� ��ȯ �罽
	ID3D11Texture2D*		depthStencilBuffer;	// ���� �� ���ٽ� ���۸� ���� 2���� �ؽ�ó
	ID3D11RenderTargetView* renderTargetView;	// ���� ���� 2���� �ؽ�ó
	ID3D11DepthStencilView* depthStencilView;	// ���� �� ���ٽ� ��
	D3D11_VIEWPORT			viewport;			// ����Ʈ

	std::wstring appTitle;			// Ŭ���̾�Ʈ â �޴� �̸�

	D3D_DRIVER_TYPE driverType;		// ������ ���� (�ϵ���� ��ġ �Ǵ� ǥ�� ��ġ)

	// Ŭ���̾�Ʈ â ũ��
	int clientWidth;				
	int clientHeight;
};

static DX3DApp* dx3dApp = 0;