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

    out << "���ܰ� �߻��߽��ϴ�." << endl << endl << "���� ���� : " <<
        string(GetType()) << "[0x" << uppercase << hex << hr << "]" << endl << "���� ���� : " <<
        GetFile() << endl << "�� ��ȣ : " + to_string(GetLine()) + "��" << endl;

    if (!info.empty())
        out << "\n[���� ����]\n" << GetErrorInfo() << endl;

    out << GetFileNameAndLine();
    exceptionText = out.str();
    return exceptionText.c_str();
}

const char* DxGraphic::HRException::GetType() const noexcept
{
    return "�׷��� ���� ����";
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
    return "�׷��� ���� (�׷��� ����̽� ���� ����)";
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

    // ImGui�� �ʱ� ������
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
    // ImGui�� �������� ó���� �ؾ� ȭ�� �� ������ ����
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

    // Device�� ������
    HRESULT hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,   // GPU �ϵ���� �������� ������ (D3D_DRIVER_TYPE_HARDWARE)
        0,
        createDeviceFlags,
        0, 0,                       // �⺻ ��� ���� �迭 (D3D_FEATURE_LEVEL_11_0�θ� ���)
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &deviceContext
    );

    // Device�� �����Ǿ����� Ȯ����
    if (FAILED(hr))
    {
        MessageBox(0, "D3D11CreateDevice Failed.", 0, 0);
        return S_FALSE;
    }

    // �׷��� �ϵ��� D3D_FEATRUE_LEVEL_11_0�� �����ϴ��� Ȯ��
    if (featureLevel != D3D_FEATURE_LEVEL_11_0)
    {
        MessageBox(0, "DirectX 3D Feature Level 11 UnSupported.", 0, 0);
        return S_FALSE;
    }

    return S_OK;
}

void DxGraphic::CheckMSAAQuality()
{
    GRAPHIC_FAILED(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality)); // MSAA�� �����Ǵ��� Ȯ��
    assert(msaaQuality > 0);                                                                            // ������ ��� 0���� ū ���� ��ȯ�Ǿ� ������
}

void DxGraphic::SwapChainSettings(HWND hWnd)
{
    DXGI_MODE_DESC backBufferDesc;

    // �ĸ� ���� ����
    backBufferDesc.Width = 0;                                                   // �ĸ� ���� �ʺ�
    backBufferDesc.Height = 0;                                                  // �ĸ� ���� ����

    backBufferDesc.RefreshRate.Denominator = 1;                                 // â �ֻ��� �и�
    backBufferDesc.RefreshRate.Numerator = 60;                                  // â �ֻ��� ����

    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // �ĸ� ���� �ȼ� ���� (���� 8��Ʈ�� �ϴ°� �����ϰ� �� ������ �ǹ̰� ����)
    backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // ȭ�� ��� ����
    backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // ȭ�� ��� ũ�� ���� ����

    DXGI_SAMPLE_DESC msaaDesc = { };

    // MSAA�� ����� ���
    if (isMSAAUsage)
    {
        msaaDesc.Count = 4;                 // 4��� Ȯ�� ��
        msaaDesc.Quality = msaaQuality - 1; // CheckMultisampleQualityLevels�� ���ؼ� ���� ��� ��
    }

    // MSAA�� ������� ���� ���
    else
    {
        msaaDesc.Count = 1;
        msaaDesc.Quality = 0;
    }

    // ���� ü�� ����
    swapChainDesc.SampleDesc = msaaDesc;
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // �ĸ� ���ۿ� �����ϱ� ���� ���� ����
    swapChainDesc.BufferCount = 1;                                  // ���� ���۸��� �ϱ� ���� 1��
    swapChainDesc.OutputWindow = hWnd;                              // ����� â ����
    swapChainDesc.Windowed = true;                                  // â ��� ���� O
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;         // ���ο� �����ӿ� �׸�
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // ȭ���� �ٲ� �� ������ �ػ󵵷� ����
}

void DxGraphic::CreateSwapChain()
{
    IDXGIDevice* dxgiDevice = nullptr;
    GRAPHIC_FAILED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IDXGIAdapter* dxgiAdapter = nullptr;
    GRAPHIC_FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    // CreateSwapChain�� ����ϱ� ���� ���� ����
    IDXGIFactory* dxgiFactory = nullptr;
    GRAPHIC_FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

    // swapChain ����
    GRAPHIC_FAILED(dxgiFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain));

    ReleaseCOM(dxgiDevice);
    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiFactory);
}

void DxGraphic::CreateRenderTargetView()
{
    ComPtr<ID3D11Texture2D> backBuffer;

    // ��ȯ �罽�� ���۸� ������ (0��° �ĸ� ���۸� ID3D11Texture2D �������� 3��° �μ��� ��ȯ)
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

    // ������ ������������ RS �ܰ迡 ViewPort ���� (1��° �Ű����� : �� ��Ʈ�� ����(�� ��Ʈ 2�� �̻��� ��� ȭ���� ���� �� ����))
    deviceContext->RSSetViewports(1, &viewPort);

    // Primitive Topology ����
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

    // Vertex Shader �ܰ踦 ������ ���������� �ܰ迡 ����
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

    // Pixel Shader �ܰ踦 ������ ���������� �ܰ迡 ����
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

    out << "���ܰ� �߻��߽��ϴ�." << endl << endl << "���� ���� : " <<
        GetType() << endl << "���� ���� : " <<
        GetFile() << endl << "�� ��ȣ : " + to_string(GetLine()) + "��" << endl;

    if (!info.empty())
        out << "\n[���� ����]\n" << GetErrorInfo() << endl;

    out << GetFileNameAndLine();
    exceptionText = out.str();
    return exceptionText.c_str();
}

const char* DxGraphic::InfoException::GetType() const noexcept
{
    return "�׷��� ���� ����";
}

string DxGraphic::InfoException::GetErrorInfo() const noexcept
{
    return info;
}
