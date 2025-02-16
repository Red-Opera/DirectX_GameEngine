#include "stdafx.h"
#include "VertexShader.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"

#include "Utility/StringConverter.h"

#include <typeinfo>
using namespace std;

namespace Graphic
{
    VertexShader::VertexShader(DxGraphic& graphic, const string& path) : path(path)
    {
        CREATEINFOMANAGER(graphic);

        DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
        shaderFlags |= D3D10_SHADER_DEBUG;
        shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

        ID3D10Blob* compiledShader = 0;
        ID3D10Blob* errorMessage = 0;

        hr = D3DX11CompileFromFileW(StringConverter::ToWide(path).c_str(), nullptr, nullptr, "VS", "vs_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

        if (FAILED(hr))
            DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);

        if (errorMessage != 0)
        {
            MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
            ReleaseCOM(errorMessage);
        }

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateVertexShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), nullptr, &vertexShader));
    }

    ID3DBlob* VertexShader::GetShaderCode() const noexcept
    {
        return shaderCode.Get();
    }

    void VertexShader::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGER(graphic);

        // Vertex Shader 단계를 렌더링 파이프라인 단계에 묶음
        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->VSSetShader(vertexShader.Get(), nullptr, 0));
    }

    std::shared_ptr<VertexShader> VertexShader::GetRender(DxGraphic& graphic, const std::string& path)
    {
        return RenderManager::GetRender<VertexShader>(graphic, path);
    }

    std::string VertexShader::CreateID(const std::string& path)
    {
        using namespace std::string_literals;
        return typeid(VertexShader).name() + "#"s + path;
    }

    std::string VertexShader::GetID() const noexcept
    {
        return CreateID(path);
    }
}