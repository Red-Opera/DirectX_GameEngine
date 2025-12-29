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
        std::wstring widePath = StringConverter::ToWString(path);
        std::wstring tempPath = L"Temp/ShaderCompile/";
        std::wstring shaderFileName = StringConverter::ToWString(StringConverter::GetFileName(path));

        namespace fileSystem = std::filesystem;

        // 컴파일된 Shader 코드를 저장하기 위한 임시 폴더 생성
        if (!fileSystem::exists(tempPath))
            fileSystem::create_directories(tempPath);

        // 원본 셰이더의 마지막 수정 시간을 확인
        auto lastWriteTime = fileSystem::last_write_time(widePath);
        auto timePoint = std::chrono::time_point_cast<std::chrono::seconds>(lastWriteTime).time_since_epoch().count();

        // 캐시 파일명 생성 (타임스탬프 포함)
        std::wstring compiledShaderPath = tempPath + shaderFileName + L"_" + std::to_wstring(timePoint) + L".cso";

        // 이전 버전의 같은 셰이더 캐시 파일 삭제
        CleanupOldShaderCaches(tempPath, shaderFileName);

		HRESULT hr = S_FALSE;
        
        // 캐시 파일이 존재하면 로드
        if (fileSystem::exists(compiledShaderPath))
        {
            hr = LoadCacheShader(compiledShaderPath);
			Require::Check(hr, ErrorCode::GRAPHICS_ShaderLoadSaveFailed, "캐시된 Pixel Shader 파일 로드 실패");
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

            hr = D3DX11CompileFromFileW(StringConverter::ToWString(path).c_str(), nullptr, nullptr, "PS", "ps_5_0", shaderFlags, 0, 0, &shaderCode, &errorMessage, nullptr);

			string errorMsg;

            if (errorMessage != nullptr)
				errorMsg = string((char*)errorMessage->GetBufferPointer());

			Require::Check(hr, ErrorCode::GRAPHICS_ShaderCompileFailed, "Graphic Pixel Shader HLSL 파일 컴파일 실패 : " + errorMsg);

            hr = SaveCacheShader(compiledShaderPath);
			Require::Check(hr, ErrorCode::GRAPHICS_ShaderLoadSaveFailed, "컴파일된 Pixel Shader 캐시 저장 실패");
        }

        hr = GetDevice(Window::GetDxGraphic())->CreatePixelShader(
            shaderCode->GetBufferPointer(),
            shaderCode->GetBufferSize(),
            nullptr,
            &pixelShader);

		Require::Check(hr, ErrorCode::GRAPHICS_ShaderCompileFailed, "Pixel Shader 생성 실패");
    }

    std::wstring Graphic::PixelShader::GetCacheFilePathWithTimestamp(const std::wstring& shaderPath, const std::wstring& basePath)
    {
        namespace fileSystem = std::filesystem;
        namespace chrono = std::chrono;

        // 원본 셰이더 파일 경로 확인
        std::wstring originalShaderPath = shaderPath;

        if (!fileSystem::exists(originalShaderPath))
            DXTraceW(__FILE__, (DWORD)__LINE__, S_FALSE, L"Shader 파일이 존재하지 않음", true);

        // 마지막 수정 시간을 초 단위로 가져옴
        auto lastWriteTime = fileSystem::last_write_time(originalShaderPath);
        auto timePoint = chrono::time_point_cast<chrono::seconds>(lastWriteTime).time_since_epoch().count();

        // 파일 이름에 타임스탬프 포함 (확장자 추출)
        std::wstring shaderFileName;
        if (originalShaderPath.find(L".cso") != std::wstring::npos) 
        {
            // 이미 처리된 파일인 경우 원본 파일명만 추출
            size_t lastSlash = originalShaderPath.find_last_of(L"/\\");
            size_t firstUnderscore = originalShaderPath.find(L"_", lastSlash != std::wstring::npos ? lastSlash + 1 : 0);

            if (firstUnderscore != std::wstring::npos) 
            {
                shaderFileName = originalShaderPath.substr
                (
                    lastSlash != std::wstring::npos ? lastSlash + 1 : 0,
                    firstUnderscore - (lastSlash != std::wstring::npos ? lastSlash + 1 : 0)
                );
            }

            else
            {
                // 언더스코어가 없으면 확장자 전까지 추출
                size_t dotPos = originalShaderPath.find_last_of(L".");

                shaderFileName = originalShaderPath.substr
                (
                    lastSlash != std::wstring::npos ? lastSlash + 1 : 0,
                    dotPos - (lastSlash != std::wstring::npos ? lastSlash + 1 : 0)
                );
            }
        }

        // 원본 셰이더 파일인 경우
        else
            shaderFileName = StringConverter::ToWString(StringConverter::GetFileName(StringConverter::ToString(originalShaderPath)));

        std::wstring timestampStr = std::to_wstring(timePoint);
        return basePath + shaderFileName + L"_" + timestampStr + L".cso";
    }

    void Graphic::PixelShader::CleanupOldShaderCaches(const std::wstring& tempPath, const std::wstring& shaderFileName)
    {
        namespace fileSystem = std::filesystem;

        try 
        {
            // 임시 폴더가 없으면 함수 종료
            if (!fileSystem::exists(tempPath))
                return;

            // 임시 폴더의 모든 파일 검사
            for (const auto& entry : fileSystem::directory_iterator(tempPath))
            {
                const auto& filePath = entry.path().wstring();

                // 현재 셰이더와 동일한 기본 이름을 가진 파일 찾기 (예: ColorShader_1234567890.cso)
                std::wstring filename = entry.path().filename().wstring();

                // 파일 이름이 "셰이더이름_숫자.cso" 형식인지 확인하여 현재 사용 중인 캐시가 아니면 삭제
                if (filename.find(shaderFileName + L"_") == 0 && filename.find(L".cso") != std::wstring::npos)
                    fileSystem::remove(entry.path());
            }
        }

        catch (const std::exception& e) 
        {
            const char* errorMessage = e.what();
        }
    }

    bool PixelShader::IsShaderCacheVaild(const std::wstring& shaderPath, const std::wstring& cachePath)
    {
        namespace fileSystem = std::filesystem;

        // 캐시 파일 이름에서 타임스탬프 추출
        std::wstring tempPath = L"Temp/ShaderCompile/";
        std::wstring expectedCachePath = GetCacheFilePathWithTimestamp(shaderPath, tempPath);

        // 실제 캐시 파일 경로가 기대하는 타임스탬프를 포함한 경로와 일치하는지 확인
        return cachePath == expectedCachePath && fileSystem::exists(cachePath);
    }
}