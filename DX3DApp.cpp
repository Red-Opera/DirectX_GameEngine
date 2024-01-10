// DX3DApp.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "DirectXSettings/DirectXSettings.h"
#include "DirectXSettings/IA.h"

float DX3DApp::displayFPSSEC = 0.5f;

bool DX3DApp::Init()
{
    if (!InitMainWindow())
        return false;

    if (!InitDX3D())
        return false;

    if (!InitPipeLine())
        return false;

    return true;
}

int DX3DApp::Run()
{
    MSG msg = { 0 };
    runningTime.Reset();

    // 클라이언트가 창을 닫을때까지 반복
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 이 부분에서 업데이트가 이루어짐
        else
        {
            runningTime.Tick();

            if (!isPause)
            {
                CalculateFrameStates();
                UpdateScene(runningTime.DeltaTime());
                DrawScene();
            }

            else
                Sleep(100);
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return dx3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

DX3DApp::DX3DApp(HINSTANCE hInstance)
    : hInstance(hInstance), appTitle(L"DirectX 3D Game"),
      driverType(D3D_DRIVER_TYPE_HARDWARE), clientWidth(800), clientHeight(600),
      isMSAAUsage(true), hWnd(0), isPause(false), isMinimized(false), isMaximized(false), isResizing(false), msaaQuality(0),

    device(0), deviceContext(0), swapChain(0), 
    depthStencilBuffer(0),renderTargetView(0), depthStencilView(0)
{
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    dx3dApp = this;
}

DX3DApp::~DX3DApp()
{
    ReleaseCOM(renderTargetView);
    ReleaseCOM(depthStencilView);
    ReleaseCOM(swapChain);
    ReleaseCOM(depthStencilBuffer);

    if (deviceContext)
        deviceContext->ClearState();

    ReleaseCOM(deviceContext);
    ReleaseCOM(device);
}

//
//  함수: InitMainWindow()
//
//  용도: 창 클래스를 등록합니다.
// 
//  함수: InitInstance(HINSTANCE, int)
//
//  용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//  주석:
//       이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//       주 프로그램 창을 만든 다음 표시합니다.
//
//
bool DX3DApp::InitMainWindow()
{
    WNDCLASS wcex;

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = MainWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = L"DirectX 3D Game";

    if (!RegisterClass(&wcex))
    {
        MessageBox(0, L"RegisterClass Failed.", 0, 0);
        return false;
    }

    RECT R = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

    int width = R.right - R.left;
    int height = R.bottom - R.top;

    hWnd = CreateWindow(L"DirectX 3D Game", appTitle.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, nullptr);

    if (!hWnd)
    {
        MessageBox(0, L"Create Failed.", 0, 0);
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return true;
}

//
//  함수: MsgProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.


LRESULT DX3DApp::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    // 프로그램이 활성화 또는 비활성화될 때 실행되는 메세지
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            // 일시 정지 여부를 참으로 설정하고 타임머를 멈춤
            isPause = true;
            runningTime.Stop();
        }

        else
        {
            isPause = false;
            runningTime.Start();
        }

        return 0;

    // 응용 프로그램 창의 크기가 바뀔 때 실행되는 메세지
    case WM_SIZE:
        // 클라이언트의 창 크기를 가져옴
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);

        if (device)
        {
            // -버튼을 눌러 창을 최소화할 경우
            if (wParam == SIZE_MINIMIZED)
            {
                isPause = true;
                isMinimized = true;
                isMaximized = true;
            }

            // ㅁ버튼을 눌러 창을 최대로 늘릴 경우
            else if (wParam == SIZE_MAXIMIZED)
            {
                // 일시정지한 상태일 수 있기 때문에 isPause를 true로 변겨
                isPause = false;
                isMinimized = false;
                isMaximized = true;
            }

            // 일반적인 창모드로 돌아왔을 경우
            else if (wParam == SIZE_RESTORED)
            {
                if (isMinimized)
                {
                    isPause = false;
                    isMinimized = false;
                    OnResize();             // 렌더 타겟 뷰와 스텐실 버퍼의 크기를 다시 바꿈
                }

                else if (isMaximized)
                {
                    isPause = false;
                    isMaximized = false;
                    OnResize();
                }

                else if (isResizing)
                {

                }

                else
                    OnResize();
            }
        }

        return 0;

    // 사용자가 Resize Bar를 잡고 이동하고 있을 경우 (확대할 때마다 화면을 갱신하는 것은 비효율적)
    case WM_ENTERSIZEMOVE:
        isPause = true;
        isResizing = true;
        runningTime.Stop();
        return 0;

    // 사용자가 Resize Bar을 잡은 후 놓을 경우
    case WM_EXITSIZEMOVE:
        isPause = false;
        isResizing = false;
        runningTime.Start();
        OnResize();
        return 0;

    // 창이 파괴될 때 실행
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MENUCHAR:
        // Alt-Enter를 누를때나는 소리를 없앰
        return MAKELRESULT(0, MNC_CLOSE);

    // 창이 너무 작아지지 않게 설정함
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;

    // 마우스 이벤트 처리
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        // GET_X_LPARAM : 이벤트가 발생한 X좌표 위치를 반환함
        OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_MOUSEMOVE:
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool DX3DApp::InitDX3D()
{
    // ========================================================
    //	Device 생성
    // ========================================================

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;         // 품질 수준
    CreateDevice(&createDeviceFlags, &featureLevel, driverType, &device, &deviceContext);

    // ========================================================
    //	4X MSAA 품질 수준 지원 점검
    // ========================================================

    CheckMSAAQuality(device, &msaaQuality);

    // ========================================================
    //	교환 사슬의 설정
    // ========================================================

    // 후면 버퍼 설정
    DXGI_MODE_DESC backBufferModeDESC;

    // MSAA를 사용하기 위한 설명서
    DXGI_SAMPLE_DESC massDESC;

    // 스왑 체인 설정
    DXGI_SWAP_CHAIN_DESC swapChainDESC;

    SwapChainSettings(dx3dApp, &backBufferModeDESC, &massDESC, &swapChainDESC);

    // ========================================================
    //	교환 사슬의 생성
    // ========================================================

    // 스왑 체인
    CreateSwapChain(device, swapChainDESC, &swapChain);

    // 뒤에 나오는 부분은 화면 크기가 조절될 때도 재사용됨
    OnResize();

    // ===============================================================================================

    // ========================================================
    //	타임머 설정
    // ========================================================

    runningTime.Reset();

    return true;
}

bool DX3DApp::InitPipeLine()
{
    IA* ia = new IA;

    return true;
}

void DX3DApp::OnResize()
{
    assert(deviceContext);
    assert(device);
    assert(swapChain);

    // 이전에 만들어진 깊이 º 스텐실 버퍼를 제거
    ReleaseCOM(renderTargetView);
    ReleaseCOM(depthStencilView);
    ReleaseCOM(depthStencilBuffer);

    // 클라이언트 창에 맞도록 버퍼 크기 변경
    HR(swapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

    // ========================================================
    //	렌더 대상 뷰의 생성
    // ========================================================

    CreateRenderTargetView(swapChain, device, &renderTargetView);

    // ========================================================
    //	깊이 º 스텐실 버퍼와 뷰 생성
    // ========================================================

    CreateDepthStencilView(dx3dApp, &depthStencilBuffer, &depthStencilView);

    // ========================================================
    //	뷰들을 출력 병합기 단계에 묶기
    // ========================================================

    // 첫번째 매개변수 : 렌더 대상 (2개 이상으로 지정할 경우 2개 이상의 장면을 동시에 렌더링함)
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // ========================================================
    //	뷰포트 설정
    // ========================================================

    CreateViewPort(dx3dApp, deviceContext, &viewport);
}

HINSTANCE DX3DApp::AppInst() const { return hInstance; }

HWND DX3DApp::MainWnd() const { return hWnd; }

float DX3DApp::AspectRatio() const
{
    return static_cast<float> (clientWidth) / clientHeight;
}

void DX3DApp::CalculateFrameStates()
{
    static int frameCount = 0;
    static float timeElapsed = 0.0f;

    frameCount++;

    if ((runningTime.TotalTime() - timeElapsed) >= displayFPSSEC)
    {
        float fps = frameCount / displayFPSSEC;  // fps = frameCount / 0.5
        float mspf = 1000.0f / fps;

        std::wostringstream outs;
        outs.precision(6);
        outs << appTitle << L"    " << L"FPS: " << fps << L"    " << L"Frame Time: " << mspf << L" (ms)";
        SetWindowText(hWnd, outs.str().c_str());

        // 초기화
        frameCount = 0;
        timeElapsed += displayFPSSEC;
    }
}