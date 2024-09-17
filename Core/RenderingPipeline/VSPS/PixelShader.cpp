#include "stdafx.h"
#include "PixelShader.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"
using namespace std;

namespace Graphic
{
    PixelShader::PixelShader(DxGraphic& graphic, const string& path) : path(path)
    {
        CREATEINFOMANAGER(graphic);

        DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
        shaderFlags |= D3D10_SHADER_DEBUG;
        shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

        ID3D10Blob* compiledShader = 0;
        ID3D10Blob* errorMessage = 0;

        hr = D3DX11CompileFromFileW(std::wstring{ path.begin(), path.end() }.c_str(), nullptr, nullptr, "PS", "ps_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

        if (FAILED(hr))
            DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);

        if (errorMessage != 0)
        {
            MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
            ReleaseCOM(errorMessage);
        }

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreatePixelShader(
            shaderCode->GetBufferPointer(),
            shaderCode->GetBufferSize(),
            nullptr,
            &pixelShader));
    }

    ID3DBlob* PixelShader::GetShaderCode() const noexcept
    {
        return shaderCode.Get();
    }

    void PixelShader::PipeLineSet(DxGraphic& graphic) noexcept
    {
        GetDeviceContext(graphic)->PSSetShader(pixelShader.Get(), nullptr, 0);
    }

    std::shared_ptr<PixelShader> PixelShader::GetRender(DxGraphic& graphic, const std::string& path)
    {
        return RenderManager::GetRender<PixelShader>(graphic, path);
    }

    std::string PixelShader::CreateID(const std::string& path)
    {
        using namespace std::literals;

        return typeid(PixelShader).name() + "$"s + path;
    }

    std::string PixelShader::GetID() const noexcept
    {
        return CreateID(path);
    }
}