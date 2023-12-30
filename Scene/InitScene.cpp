#include "stdafx.h"
#include <xnamath.h>

class InitScene : public DX3DApp
{
public:
	InitScene(HINSTANCE hInstance);
	~InitScene();

	bool Init();
	void OnResize();
	
	// DX3DApp을(를) 통해 상속됨
	void UpdateScene(float dt) override;
	void DrawScene() override;
};

// 프로그램 중 가장 먼저 실행되는 함수
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// 디버그 빌드의 경우 실행시점 메모리 점검 기능을 견다.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	InitScene initScene(hInstance);

	if (!initScene.Init())
		return 0;

	return initScene.Run();
}

InitScene::InitScene(HINSTANCE hInstance) : DX3DApp(hInstance)
{
}

InitScene::~InitScene()
{
}

bool InitScene::Init()
{
	if (!DX3DApp::Init())
		return false;

	return true;
}

void InitScene::OnResize()
{
	DX3DApp::OnResize();
}

void InitScene::UpdateScene(float dt)
{
}

void InitScene::DrawScene()
{
	assert(deviceContext);
	assert(swapChain);

	// 후면 버퍼를 초기화함
	deviceContext->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Blue));

	// 깊이 버퍼를 1.0f, 스텐실 버퍼를 0으로 지운다.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 후면 버퍼를 화면에 표시한다.
	HR(swapChain->Present(0, 0));
}
