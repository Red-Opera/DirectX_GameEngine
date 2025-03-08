#include "stdafx.h"
#include "VertexShader.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"

#include "Utility/StringConverter.h"

#include <typeinfo>
using namespace std;

namespace Graphic
{
    VertexShader::VertexShader(const string& path) : path(path)
    {
        CREATEINFOMANAGER(Window::GetDxGraphic());

        DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
        shaderFlags |= D3D10_SHADER_DEBUG;
        shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

        ID3D10Blob* compiledShader = 0;
        ID3D10Blob* errorMessage = 0;

        hr = D3DX11CompileFromFileW(StringConverter::ToWide(path).c_str(), nullptr, nullptr, "VS", "vs_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

        if (errorMessage != 0)
        {
            MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
            ReleaseCOM(errorMessage);
        }

        GRAPHIC_THROW_INFO(GetDevice(Window::GetDxGraphic())->CreateVertexShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), nullptr, &vertexShader));
    }

    ID3DBlob* VertexShader::GetShaderCode() const noexcept
    {
        return shaderCode.Get();
    }

    void VertexShader::SetRenderPipeline() NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

        // Vertex Shader 단계를 렌더링 파이프라인 단계에 묶음
        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->VSSetShader(vertexShader.Get(), nullptr, 0));
    }

    std::shared_ptr<VertexShader> VertexShader::GetRender(const std::string& path)
    {
        return RenderManager::GetRender<VertexShader>(path);
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