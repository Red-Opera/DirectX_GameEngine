#include "stdafx.h"
#include "DxGraphic.h"
#include "dxerr.h"
#include "dxgi.h"

#include "Exception/GraphicsException.h"
#include "RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "RenderingPipeline/RenderTarget.h"
#include "Utility/Imgui/imgui_impl_dx11.h"
#include "Utility/Imgui/imgui_impl_win32.h"

#include <array>
using namespace std;

DxGraphic::HRException::HRException(int line, const char* file, HRESULT hr, vector<string> infoMessage) noexcept :
    Exception(line, file), hr(hr)
{
    for (const auto& message : infoMessage)
    {
        info += message;
        info.push_back('\n');
    }

    if (!info.empty())
        info.pop_back();
}

DxGraphic::HRException::HRException(int line, string file, HRESULT hr, vector<string> infoMessage) noexcept :
    Exception(line, file), hr(hr)
{
    for (const auto& message : infoMessage)
    {
        info += message;
        info.push_back('\n');
    }

    if (!info.empty())
        info.pop_back();
}

const char* DxGraphic::HRException::what() const noexcept
{
    ostringstream out;

    out << "예외가 발생했습니다." << endl << endl << "예외 종류 : " <<
        string(GetType()) << "[0x" << uppercase << hex << hr << "]" << endl << "예외 파일 : " <<
        GetFile() << endl << "줄 번호 : " + to_string(GetLine()) + "줄" << endl;

    if (!info.empty())
        out << "\n[에러 정보]\n" << GetErrorInfo() << endl;

    out << GetFileNameAndLine();
    exceptionText = out.str();
    return exceptionText.c_str();
}

const char* DxGraphic::HRException::GetType() const noexcept
{
    return "그래픽 생성 문제";
}

HRESULT DxGraphic::HRException::GetErrorCode() const noexcept
{
    return hr;
}

string DxGraphic::HRException::GetExptionContent() const noexcept
{
    return DXGetErrorString(hr);
}

string DxGraphic::HRException::GetErrorDescription() const noexcept
{
    string errorMessage = DXGetErrorDescription(hr);

    return errorMessage;
}

string DxGraphic::HRException::GetErrorInfo() const noexcept
{
    return info;
}

const char* DxGraphic::RemoveException::GetType() const noexcept
{
    return "그래픽 예외 (그래픽 디바이스 제거 문제)";
}

DxGraphic::DxGraphic(HWND hWnd)
{
    width = WINWIDTH;
    height = WINHEIGHT;

	CreateDevice();
    CheckMSAAQuality();
    SwapChainSettings(hWnd);
    CreateSwapChain();
    CreateRenderTargetView();

    // ImGui를 초기 설정함
    ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
}

void DxGraphic::SetProjection(DirectX::XMMATRIX projection) noexcept
{
    this->projection = projection;
}

void DxGraphic::BeginFrame(float red, float green, float blue) noexcept
{
    if (imGuiEnable)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }
}

void DxGraphic::EndFrame()
{
    // ImGui는 마지막에 처리를 해야 화면 맨 앞으로 나옴
    if (imGuiEnable)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    HRESULT hr = swapChain->Present(1, 0);

#ifndef NDEBUG
    infoManager.Set();
#endif // !NDEBUG


    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GRAPHIC_REMOVE_EXCEPT(device->GetDeviceRemovedReason());
        }

        else
        {
            throw GRAPHIC_EXCEPT(hr);
        }
    }
}

UINT DxGraphic::GetWidth() const noexcept
{
    return width;
}

UINT DxGraphic::GetHeight() const noexcept
{
    return height;
}

std::shared_ptr<Graphic::RenderTarget> DxGraphic::GetRenderTarget()
{
    return renderTarget;
}

void DxGraphic::EnableImGui() noexcept
{
    imGuiEnable = true;
}

void DxGraphic::DisableImGui() noexcept
{
    imGuiEnable = false;
}

bool DxGraphic::IsImGuiEnable() noexcept
{
    return imGuiEnable;
}

HRESULT DxGraphic::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevel;
	UINT createDeviceFlags = 0;

#if defined (DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Device를 생성함
    HRESULT hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,   // GPU 하드웨어 가속으로 실행함 (D3D_DRIVER_TYPE_HARDWARE)
        0,
        createDeviceFlags,
        0, 0,                       // 기본 기능 수준 배열 (D3D_FEATURE_LEVEL_11_0로만 사용)
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &deviceContext
    );

    // Device가 생성되었는지 확인함
    if (FAILED(hr))
    {
        MessageBox(0, "D3D11CreateDevice Failed.", 0, 0);
        return S_FALSE;
    }

    // 그래픽 하드웨어가 D3D_FEATRUE_LEVEL_11_0을 지원하는지 확인
    if (featureLevel != D3D_FEATURE_LEVEL_11_0)
    {
        MessageBox(0, "DirectX 3D Feature Level 11 UnSupported.", 0, 0);
        return S_FALSE;
    }

    return S_OK;
}

void DxGraphic::CheckMSAAQuality()
{
    GRAPHIC_FAILED(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality)); // MSAA가 지원되는지 확인
    assert(msaaQuality > 0);                                                                            // 지원할 경우 0보다 큰 값이 반환되어 성공함
}

void DxGraphic::SwapChainSettings(HWND hWnd)
{
    DXGI_MODE_DESC backBufferDesc;

    // 후면 버퍼 설정
    backBufferDesc.Width = 0;                                                   // 후면 버퍼 너비
    backBufferDesc.Height = 0;                                                  // 후면 버퍼 높이

    backBufferDesc.RefreshRate.Denominator = 1;                                 // 창 주사율 분모
    backBufferDesc.RefreshRate.Numerator = 60;                                  // 창 주사율 분자

    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // 후면 버퍼 픽셀 형식 (보통 8비트씩 하는게 적당하고 더 높여도 의미가 없음)
    backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // 화면 출력 순서
    backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // 화면 출력 크기 조절 여부

    DXGI_SAMPLE_DESC msaaDesc = { };

    // MSAA를 사용할 경우
    if (isMSAAUsage)
    {
        msaaDesc.Count = 4;                 // 4배로 확장 함
        msaaDesc.Quality = msaaQuality - 1; // CheckMultisampleQualityLevels로 통해서 얻어온 결과 값
    }

    // MSAA를 사용하지 않을 경우
    else
    {
        msaaDesc.Count = 1;
        msaaDesc.Quality = 0;
    }

    // 스왑 체인 설정
    swapChainDesc.SampleDesc = msaaDesc;
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // 후면 버퍼에 렌더하기 위해 값을 설정
    swapChainDesc.BufferCount = 1;                                  // 더블 버퍼링을 하기 위해 1로
    swapChainDesc.OutputWindow = hWnd;                              // 출력할 창 설정
    swapChainDesc.Windowed = true;                                  // 창 모드 여부 O
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;         // 새로운 프레임에 그림
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // 화면이 바뀔 때 적절한 해상도로 설정
}

void DxGraphic::CreateSwapChain()
{
    IDXGIDevice* dxgiDevice = nullptr;
    GRAPHIC_FAILED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IDXGIAdapter* dxgiAdapter = nullptr;
    GRAPHIC_FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    // CreateSwapChain을 사용하기 위한 변수 생성
    IDXGIFactory* dxgiFactory = nullptr;
    GRAPHIC_FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

    // swapChain 생성
    GRAPHIC_FAILED(dxgiFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain));

    ReleaseCOM(dxgiDevice);
    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiFactory);
}

void DxGraphic::CreateRenderTargetView()
{
    ComPtr<ID3D11Texture2D> backBuffer;

    // 교환 사슬의 버퍼를 가져옴 (0번째 후면 버퍼를 ID3D11Texture2D 형식으로 3번째 인수로 반환)
    GRAPHIC_THROW_INFO(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

    renderTarget = std::shared_ptr<Graphic::RenderTarget>{ new Graphic::OutputOnlyRenderTarget(*this, backBuffer.Get()) };

    D3D11_VIEWPORT viewport;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    deviceContext->RSSetViewports(1u, &viewport);
}

void DxGraphic::DrawTestTriangle(float angle, float x, float z)
{
    struct Vertex
    {
        struct
        {
            float x;
            float y;
            float z;
        } pos;
    };

    Vertex vertices[] =
    {
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f  },
        { -1.0f, 1.0f, -1.0f  },
        { 1.0f, 1.0f, -1.0f   },
        { -1.0f, -1.0f, 1.0f  },
        { 1.0f, -1.0f, 1.0f   },
        { -1.0f, 1.0f, 1.0f   },
        { 1.0f, 1.0f, 1.0f    },

    };

    ComPtr<ID3D11Buffer> vertexBuffer;
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.ByteWidth = sizeof(vertices);
    vertexBufferDesc.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA initData = { };
    initData.pSysMem = vertices;

    GRAPHIC_THROW_INFO(device->CreateBuffer(&vertexBufferDesc, &initData, &vertexBuffer));

    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    const unsigned short indices[] =
    {
        0, 2, 1, 2, 3, 1,
        1, 3, 5, 3, 7, 5,
        2, 6, 3, 3, 6, 7,
        4, 5, 7, 4, 7, 6,
        0, 4, 2, 2, 4, 6,
        0, 1, 4, 1, 5, 4
    };

    ComPtr<ID3D11Buffer> indexBuffer;
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.ByteWidth = sizeof(UINT) * 36;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = indices;

    HR(device->CreateBuffer(&indexBufferDesc, &initData, &indexBuffer));
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    D3D11_VIEWPORT viewPort;
    viewPort.TopLeftX = 0.0f;
    viewPort.TopLeftY = 0.0f;
    viewPort.Width = WINWIDTH;
    viewPort.Height = WINHEIGHT;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;

    // 렌더링 파이프라인의 RS 단계에 ViewPort 설정 (1번째 매개변수 : 뷰 포트의 개수(뷰 포트 2개 이상할 경우 화면을 나눌 수 있음))
    deviceContext->RSSetViewports(1, &viewPort);

    // Primitive Topology 설정
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // =================================
    //	Create Vertex Shader
    // =================================


    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3DBlob> shaderCode;

    DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
    shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    ID3D10Blob* compiledShader = 0;
    ID3D10Blob* errorMessage = 0;

    HRESULT hr = D3DX11CompileFromFileA("Shader/ColorShader.hlsl", nullptr, nullptr, "VS", "vs_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

    if (FAILED(hr))
        DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);

    if (errorMessage != 0)
    {
        MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
        ReleaseCOM(errorMessage);
    }

    GRAPHIC_THROW_INFO(device->CreateVertexShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), nullptr, &vertexShader));

    // Vertex Shader 단계를 렌더링 파이프라인 단계에 묶음
    deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);

    ComPtr<ID3D11InputLayout> inputLayout;
    const D3D11_INPUT_ELEMENT_DESC parameter[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    device->CreateInputLayout(parameter, (UINT)size(parameter), shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), &inputLayout);

    deviceContext->IASetInputLayout(inputLayout.Get());

    // =================================
    //	Create Vertex Constant Buffer
    // =================================

    struct VertexConstantBuffer
    {
        DxMath::XMMATRIX transform;
    };

    const VertexConstantBuffer cb =
    {
        //DxMath::XMMatrixMultiply(DxMath::XMMatrixRotationZ(angle), DxMath::XMMatrixScaling(3.0f / 4.0f, 1.0f, 1.0f))
        DxMath::XMMatrixTranspose(
                DxMath::XMMatrixRotationZ(angle) * 
                DxMath::XMMatrixRotationX(angle) * 
                DxMath::XMMatrixTranslation(x, 0.0f, z + 4.0f) *
                DxMath::XMMatrixPerspectiveFovLH(1.0f, 4.0f / 3.0f, 0.5f, 10.0f))
    };

    ComPtr<ID3D11Buffer> constantBuffer;
    D3D11_BUFFER_DESC constantBufferDesc;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.ByteWidth = sizeof(cb);
    constantBufferDesc.StructureByteStride = 0;

    initData.pSysMem = &cb;
    GRAPHIC_THROW_INFO(device->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer));

    deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // =================================
    //	Create Pixel Shader
    // =================================

    ComPtr<ID3D11PixelShader> pixelShader;

    compiledShader = 0;
    errorMessage = 0;

    hr = D3DX11CompileFromFileA("Shader/ColorShader.hlsl", nullptr, nullptr, "PS", "ps_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

    if (FAILED(hr))
        DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);

    if (errorMessage != 0)
    {
        MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
        ReleaseCOM(errorMessage);
    }
    device->CreatePixelShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), nullptr, &pixelShader);

    // Pixel Shader 단계를 렌더링 파이프라인 단계에 묶음
    deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    // =================================
    //	Create Pixel Constant Buffer
    // =================================

    struct PixelConstnatBuffer
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        } faceColor[6];
    };

    const PixelConstnatBuffer cb2 =
    {
        {
            { 1.0f, 0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 1.0f, 1.0f }
        }
    };

    ComPtr<ID3D11Buffer> pixelConstantBuffer;
    D3D11_BUFFER_DESC pixelConstantBufferDesc;
    pixelConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pixelConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    pixelConstantBufferDesc.CPUAccessFlags = 0;
    pixelConstantBufferDesc.MiscFlags = 0;
    pixelConstantBufferDesc.ByteWidth =  sizeof(cb2);
    pixelConstantBufferDesc.StructureByteStride = 0;
    
    D3D11_SUBRESOURCE_DATA pixelInitData = {};
    pixelInitData.pSysMem = &cb2;

    GRAPHIC_THROW_INFO(device->CreateBuffer(&pixelConstantBufferDesc, &pixelInitData, &pixelConstantBuffer));
    deviceContext->PSSetConstantBuffers(0, 1, pixelConstantBuffer.GetAddressOf());

    GRAPHIC_THROW_INFO_ONLY(deviceContext->DrawIndexed((UINT)size(indices), 0, 0));
}

void DxGraphic::DrawIndexed(UINT count) NOEXCEPTRELEASE
{
    GRAPHIC_THROW_INFO_ONLY(deviceContext->DrawIndexed(count, 0, 0));
}

DxGraphic::InfoException::InfoException(int line, const char* file, vector<string> infoMessage)
    : Exception(line, file)
{
    for (const auto& m : infoMessage)
    {
        info += m;
     
        info.push_back('\n');
    }

    if (!info.empty())
        info.pop_back();
}

DxGraphic::InfoException::InfoException(int line, string file, vector<string> infoMessage)
    : Exception(line, file)
{
    for (const auto& m : infoMessage)
    {
        info += m;

        info.push_back('\n');
    }

    if (!info.empty())
        info.pop_back();
}

const char* DxGraphic::InfoException::what() const noexcept
{
    ostringstream out;

    out << "예외가 발생했습니다." << endl << endl << "예외 종류 : " <<
        GetType() << endl << "예외 파일 : " <<
        GetFile() << endl << "줄 번호 : " + to_string(GetLine()) + "줄" << endl;

    if (!info.empty())
        out << "\n[에러 정보]\n" << GetErrorInfo() << endl;

    out << GetFileNameAndLine();
    exceptionText = out.str();
    return exceptionText.c_str();
}

const char* DxGraphic::InfoException::GetType() const noexcept
{
    return "그래픽 생성 문제";
}

string DxGraphic::InfoException::GetErrorInfo() const noexcept
{
    return info;
}
