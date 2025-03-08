#include "stdafx.h"
#include "PixelShader.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"

#include "Utility/StringConverter.h"

#include <chrono>
#include <filesystem>
#include <fstream>

using namespace std;

namespace Graphic
{
    PixelShader::PixelShader(const string& path) : path(path)
    {
        GetCompileShader(path);
    }

    ID3DBlob* PixelShader::GetShaderCode() const noexcept
    {
        return shaderCode.Get();
    }

    void PixelShader::SetRenderPipeline() NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetShader(pixelShader.Get(), nullptr, 0));
    }

    std::shared_ptr<PixelShader> PixelShader::GetRender(const std::string& path)
    {
        return RenderManager::GetRender<PixelShader>(path);
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

    HRESULT PixelShader::LoadCacheShader(const std::wstring& cacheShaderPath)
    {
        return D3DReadFileToBlob(cacheShaderPath.c_str(), &shaderCode);
    }

    HRESULT Graphic::PixelShader::SaveCacheShader(const std::wstring& tempShaderPath)
    {
        std::ofstream output(tempShaderPath, std::ios::binary);

        if (!output)
            return E_FAIL;

        output.write(reinterpret_cast<const char*>(shaderCode->GetBufferPointer()), shaderCode->GetBufferSize());
        output.close();

        return S_OK;
    }

    void PixelShader::GetCompileShader(const std::string& path)
    {
        CREATEINFOMANAGER(Window::GetDxGraphic());

        std::wstring widePath = StringConverter::ToWide(path);
        std::wstring tempPath = L"Temp/ShaderCompile/";
        std::wstring shaderFileName = StringConverter::ToWide(StringConverter::GetFileName(path));
        std::wstring compiledShaderPath = tempPath + shaderFileName + L".cso";

        namespace fileSystem = std::filesystem;

        // 컴파일된 Shader 코드를 저장하기 위한 임시 폴더 생성
        if (!fileSystem::exists(tempPath))
            fileSystem::create_directories(tempPath);

        hr = E_FAIL;

        // 컴파일된 Shader 코드가 존재한 경우 컴파일된 cso 파일을 가져옴
        if (fileSystem::exists(compiledShaderPath) && IsShaderCacheVaild(compiledShaderPath, widePath))
        {
            hr = LoadCacheShader(compiledShaderPath);

            GRAPHIC_EXCEPT_INFO(hr);
        }

        if (hr != S_OK)
        {
            DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
            shaderFlags |= D3D10_SHADER_DEBUG;
            shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

            ID3D10Blob* compiledShader = 0;
            ID3D10Blob* errorMessage = 0;

            hr = D3DX11CompileFromFileW(StringConverter::ToWide(path).c_str(), nullptr, nullptr, "PS", "ps_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

            if (FAILED(hr))
                DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);

            if (errorMessage != 0)
            {
                MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
                ReleaseCOM(errorMessage);
            }

            hr = SaveCacheShader(compiledShaderPath);
            GRAPHIC_THROW_INFO(hr);
        }

        GRAPHIC_THROW_INFO(GetDevice(Window::GetDxGraphic())->CreatePixelShader(
            shaderCode->GetBufferPointer(),
            shaderCode->GetBufferSize(),
            nullptr,
            &pixelShader));
    }

    bool PixelShader::IsShaderCacheVaild(const std::wstring& shaderPath, const std::wstring& cachePath)
    {
        namespace fileSystem = std::filesystem;

        if (!fileSystem::exists(shaderPath) || !fileSystem::exists(cachePath))
            DXTrace(__FILE__, (DWORD)__LINE__, S_FALSE, "Shader 파일과 캐시 경로가 존재하지 않음", true);

        auto shaderCompileTime = fileSystem::last_write_time(shaderPath);
        auto chcheCompileTime = fileSystem::last_write_time(cachePath);

        return shaderCompileTime >= chcheCompileTime;
    }
}