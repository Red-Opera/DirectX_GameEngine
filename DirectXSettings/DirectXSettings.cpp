#include "stdafx.h"
#include "DirectXSettings.h"

HRESULT CreateDevice(UINT* createDeviceFlags, D3D_FEATURE_LEVEL* featureLevel, D3D_DRIVER_TYPE driverType, ID3D11Device** device, ID3D11DeviceContext** deviceContext)
{
#if defined (DEBUG) || defined(_DEBUG)
    *createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Device를 생성함
    HRESULT hr = D3D11CreateDevice(
        0,
        driverType,         // GPU 하드웨어 가속으로 실행함 (D3D_DRIVER_TYPE_HARDWARE)
        0,
        *createDeviceFlags,
        0, 0,               // 기본 기능 수준 배열 (D3D_FEATURE_LEVEL_11_0로만 사용)
        D3D11_SDK_VERSION,
        device,
        featureLevel,
        deviceContext
    );

    // Device가 생성되었는지 확인함
    if (FAILED(hr))
    {
        MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
        return S_FALSE;
    }

    // 그래픽 하드웨어가 D3D_FEATRUE_LEVEL_11_0을 지원하는지 확인
    if (*featureLevel != D3D_FEATURE_LEVEL_11_0)
    {
        MessageBox(0, L"DirectX 3D Feature Level 11 UnSupported.", 0, 0);
        return S_FALSE;
    }

    return S_OK;
}

void CheckMSAAQuality(ID3D11Device* device, UINT* msaaQuality)
{
    HR(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, msaaQuality));  // MSAA가 지원되는지 확인
    assert(*msaaQuality > 0);                                                                // 지원할 경우 0보다 큰 값이 반환되어 성공함
}

void SwapChainSettings(DX3DApp* dx3dApp, DXGI_MODE_DESC* backBufferModeDESC, DXGI_SAMPLE_DESC* massDESC, DXGI_SWAP_CHAIN_DESC* swapChainDESC)
{
    // 후면 버퍼 설정
    backBufferModeDESC->Width = dx3dApp->GetClientWidth();                          // 후면 버퍼 너비
    backBufferModeDESC->Height = dx3dApp->GetClientHeight();                        // 후면 버퍼 높이

    backBufferModeDESC->RefreshRate.Denominator = 1;                                 // 창 주사율 분모
    backBufferModeDESC->RefreshRate.Numerator = 60;                                  // 창 주사율 분자

    backBufferModeDESC->Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // 후면 버퍼 픽셀 형식 (보통 8비트씩 하는게 적당하고 더 높여도 의미가 없음)
    backBufferModeDESC->ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // 화면 출력 순서
    backBufferModeDESC->Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // 화면 출력 크기 조절 여부

    // MSAA를 사용할 경우
    if (dx3dApp->GetUsageMSAA())
    {
        massDESC->Count = 4;                               // 4배로 확장 함
        massDESC->Quality = dx3dApp->GetMSAAQuality() - 1; // CheckMultisampleQualityLevels로 통해서 얻어온 결과 값
    }

    // MSAA를 사용하지 않을 경우
    else
    {
        massDESC->Count = 1;
        massDESC->Quality = 0;
    }

    // 스왑 체인 설정
    swapChainDESC->SampleDesc = *massDESC;
    swapChainDESC->BufferDesc = *backBufferModeDESC;
    swapChainDESC->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // 후면 버퍼에 렌더하기 위해 값을 설정
    swapChainDESC->BufferCount = 1;                                  // 더블 버퍼링을 하기 위해 1로
    swapChainDESC->OutputWindow = dx3dApp->MainWnd();                // 출력할 창 설정
    swapChainDESC->Windowed = true;                                  // 창 모드 여부 O
    swapChainDESC->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;            // 새로운 프레임에 그림
    swapChainDESC->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // 화면이 바뀔 때 적절한 해상도로 설정
}

void CreateSwapChain(ID3D11Device* device, DXGI_SWAP_CHAIN_DESC swapChainDESC, IDXGISwapChain** swapChain)
{
    IDXGIDevice* dxgiDevice = nullptr;
    HR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IDXGIAdapter* dxgiAdapter = nullptr;
    HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    // CreateSwapChain을 사용하기 위한 변수 생성
    IDXGIFactory* dxgiFactory = nullptr;
    HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

    // swapChain 생성
    HR(dxgiFactory->CreateSwapChain(device, &swapChainDESC, swapChain));

    ReleaseCOM(dxgiDevice);
    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiFactory);
}

void CreateRenderTargetView(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11RenderTargetView** renderTargetView)
{
    ID3D11Texture2D* backBuffer;
    
    // 교환 사슬의 버퍼를 가져옴 (0번째 후면 버퍼를 ID3D11Texture2D 형식으로 3번째 인수로 반환)
    HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

    // 후면 버퍼로 RenderTargetView를 생성함
    device->CreateRenderTargetView(backBuffer, 0, renderTargetView);
    
    ReleaseCOM(backBuffer);
}

void CreateDepthStencilView(DX3DApp* dx3dApp, ID3D11Texture2D** depthStencilBuffer, ID3D11DepthStencilView** depthStencilView)
{
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = dx3dApp->GetClientWidth();
    depthStencilDesc.Height = dx3dApp->GetClientHeight();
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;                             // 깊이 º 스텐실 버퍼는 한개의 2차원 배열을 사용
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (dx3dApp->GetUsageMSAA())
    {
        depthStencilDesc.SampleDesc.Count = 4;
        depthStencilDesc.SampleDesc.Quality = dx3dApp->GetMSAAQuality() - 1;
    }

    else
    {
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
    }

    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    HR(dx3dApp->GetDevice()->CreateTexture2D(&depthStencilDesc, 0, depthStencilBuffer));          // 2번째 매개변수 : 2D 텍스처에 기본적으로 입력될 값 (DepthStenil은 기본적으로 DirectX가 채워 줌)
    HR(dx3dApp->GetDevice()->CreateDepthStencilView(*depthStencilBuffer, 0, depthStencilView));   // 2번째 매개변수 : View DESC 값
}

void CreateViewPort(DX3DApp* dx3dApp, ID3D11DeviceContext* deviceContext, D3D11_VIEWPORT* viewPort)
{
    viewPort->TopLeftX = 0.0f;
    viewPort->TopLeftY = 0.0f;
    viewPort->Width = static_cast<float> (dx3dApp->GetClientWidth());
    viewPort->Height = static_cast<float> (dx3dApp->GetClientHeight());
    viewPort->MinDepth = 0.0f;
    viewPort->MaxDepth = 1.0f;

    // 렌더링 파이프라인의 RS 단계에 ViewPort 설정 (1번째 매개변수 : 뷰 포트의 개수(뷰 포트 2개 이상할 경우 화면을 나눌 수 있음))
    deviceContext->RSSetViewports(1, viewPort);
}
