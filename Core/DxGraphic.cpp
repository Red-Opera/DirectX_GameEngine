#include "stdafx.h"
#include "DxGraphic.h"
#include "dxgi.h"

#include "Core/Window.h"
#include "Exception/GraphicsException.h"
#include "RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "RenderingPipeline/RenderTarget.h"

#include "External/Imgui/imgui_impl_dx11.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "Utility/Imgui/ImguiManager.h"

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

DxGraphic::HRException::HRException(int line, const char* file, HRESULT hr, const char* infoMessage) noexcept
    : Exception(line, file), hr(hr)
{
    if (infoMessage)
        info = infoMessage;
}

DxGraphic::HRException::HRException(int line, std::string file, HRESULT hr, const std::string infoMessage) noexcept 
	: Exception(line, file), hr(hr)
{
    if (!infoMessage.empty())
		info = infoMessage;
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
    _com_error err(hr);
    return "HRESULT Error : " + string(err.ErrorMessage());
}

string DxGraphic::HRException::GetErrorDescription() const noexcept
{
	_com_error err(hr);

#ifdef UNICODE
    // 유니코드 환경일 경우를 대비한 변환 (일반적인 프로젝트 설정)
	_bst_r bstr(err.ErrorMessage());

	return string((const char*)bstr);

#else
	return string(err.ErrorMessage());
#endif // UNICODE
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

    // ImGui를 초기 설정함
    ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

    ID3D11ShaderResourceView* const nullTexture = nullptr;
    deviceContext->PSSetShaderResources(0, 1, &nullTexture);
    deviceContext->PSSetShaderResources(3, 1, &nullTexture);
}

void DxGraphic::EndFrame()
{
    // ImGui는 마지막에 처리를 해야 화면 맨 앞으로 나옴
    if (imGuiEnable)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    HRESULT hr = swapChain->Present(0, 0);

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

#ifndef NDEBUG
	// Debug 모드에서 디버그 레이어 활성화
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Device를 생성함
    HRESULT hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,   // GPU 하드웨어 가속으로 실행함 (D3D_DRIVER_TYPE_HARDWARE)
        0,
        createDeviceFlags,
        0, 0,                       // 기본 기능 수준 배열 (D3D_FEATURE_LEVEL_11_0으로만 사용)
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
    backBufferDesc.RefreshRate.Numerator = 0;                                  // 창 주사율 분자

    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // 후면 버퍼 픽셀 형식 (보통 8비트씩 하는게 적당하고 더 높여도 의미가 없음)
    backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // 화면 출력 순서
    backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // 화면 출력 크기 조절 여부

    DXGI_SAMPLE_DESC msaaDesc = { };

    // Flip-model은 MSAA를 지원하지 않음 (항상 1로 설정해야 함)
    msaaDesc.Count = 1;
    msaaDesc.Quality = 0;

    // 스왑 체인 설정
    swapChainDesc.SampleDesc = msaaDesc;
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // 후면 버퍼에 렌더하기 위해 값을 설정
    swapChainDesc.BufferCount = 2;                                  // Flip-model은 최소 2개의 버퍼 필요
    swapChainDesc.OutputWindow = hWnd;                              // 출력할 창 설정
    swapChainDesc.Windowed = true;                                  // 창 모드 여부 O
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;       // 최신 Flip-model 사용 (성능 향상)
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

	Require::Check([&] { dxgiFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain); }, ErrorCode::GRAPHICS_BufferCreateFailed, "스왑 체인 생성 실패");

    ReleaseCOM(dxgiDevice);
    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiFactory);
}

void DxGraphic::CreateRenderTargetView()
{
    // 교환 사슬의 버퍼를 가져옴 (0번째 후면 버퍼를 ID3D11Texture2D 형식으로 3번째 인수로 반환)
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	Require::Check(hr, ErrorCode::GRAPHICS_GetBufferFailed, "후면 버퍼 가져오기 실패");

    renderTarget = std::shared_ptr<Graphic::RenderTarget>{ new Graphic::OutputOnlyRenderTarget(backBuffer.Get()) };

    D3D11_VIEWPORT viewport;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    deviceContext->RSSetViewports(1u, &viewport);
}

void DxGraphic::DrawIndexed(UINT count) NOEXCEPTRELEASE
{
    Require::Check([&] { deviceContext->DrawIndexed(count, 0, 0); }, ErrorCode::GRAPHICS_DrawCallFailed, "인덱스 드로우 호출 실패");
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

DxGraphic::InfoException::InfoException(int line, const char* file, const char* infoMessage)
    : Exception(line, file)
{
    if (infoMessage)
        info = infoMessage;
}

DxGraphic::InfoException::InfoException(int line, std::string file, const std::string infoMessage) 
    : Exception(line, file)
{
    if (!infoMessage.empty())
		info = infoMessage;
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
