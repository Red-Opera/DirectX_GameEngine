#include "stdafx.h"
#include "Require.h"

#include <chrono>
#include <comdef.h>

#ifndef NDEBUG
#include "Core/Exception/ExceptionInfo.h"
#include "Core/DxGraphic.h"
#include <dxgidebug.h>

#pragma comment(lib, "dxguid.lib")
#endif

void Require::Check(bool isTrue, ErrorCode errorCode, const char* message, const std::source_location& location)
{
    if (isTrue)
        return;

    std::ostringstream oss;

    // 헤더
    oss << "\n";
    oss << "+================================================================+\n";
    oss << "|                        ERROR OCCURRED                          |\n";
    oss << "+================================================================+\n";

    // 에러 코드
    std::string errorCodeStr = errorCodeStrings[static_cast<size_t>(errorCode)];
    oss << "| Error Code : " << std::left << std::setw(47) << errorCodeStr << "|\n";

    // 메시지
    if (message != nullptr)
        oss << "| Message    : " << std::left << std::setw(47) << message << "|\n";


    const char* fileName = location.file_name();

    // 위치 정보
    if (fileName != nullptr)
    {
        // 파일 경로에서 파일명만 추출
        std::string fileName = location.file_name();
        size_t lastSlash = fileName.find_last_of("\\/");

        if (lastSlash != std::string::npos)
            fileName = fileName.substr(lastSlash + 1);

        oss << "+----------------------------------------------------------------+\n";
        oss << "| File       : " << std::left << std::setw(47) << fileName << "|\n";
        oss << "| Line       : " << std::left << std::setw(47) << location.line() << "|\n";

        std::string functionName = location.function_name();

        if (!functionName.empty())
            oss << "| Function   : " << std::left << std::setw(47) << functionName << "|\n";
    }

    // 타임스탬프
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char timeStr[26];

    ctime_s(timeStr, sizeof(timeStr), &time);
    timeStr[24] = '\0'; // 개행 문자 제거

    oss << "+----------------------------------------------------------------+\n";
    oss << "| Time       : " << std::left << std::setw(47) << timeStr << "|\n";

    // 푸터
    oss << "+================================================================+\n";

    std::string result = oss.str();

    // Visual Studio Output 창에 출력
    OutputDebugStringA(result.c_str());

    // 콘솔 출력
    std::cerr << result;

    // 디버거 연결 여부에 따라 처리
    if (IsDebuggerPresent())
        __debugbreak(); // 디버거에서 즉시 중단

    else
    {
        MessageBoxA(nullptr, result.c_str(), "Critical Error", MB_OK | MB_ICONERROR);
        std::abort();
    }
}

void Require::Check(HRESULT hr, ErrorCode errorCode, const char* message, const std::source_location& location)
{
    if (!FAILED(hr))
        return;

    // HRESULT 에러 메시지 가져오기
    _com_error err(hr);
    std::string hrMessage = "HRESULT: 0x";

    char hexStr[16];
    sprintf_s(hexStr, "%08X", static_cast<unsigned int>(hr));
    hrMessage += hexStr;
    hrMessage += " - ";

    // COM 에러 설명 추가
    LPCTSTR errMsg = err.ErrorMessage();

    if (errMsg != nullptr)
    {
#ifdef UNICODE
        // 유니코드를 멀티바이트로 변환
        char buffer[512];
        WideCharToMultiByte(CP_ACP, 0, errMsg, -1, buffer, sizeof(buffer), NULL, NULL);
        hrMessage += buffer;
#else
        hrMessage += errMsg;
#endif
    }

    // 사용자 메시지와 HRESULT 메시지 결합
    std::string fullMessage;

    if (message != nullptr)
        fullMessage = std::string(message) + " | " + hrMessage;

    else
        fullMessage = hrMessage;

    Check(false, errorCode, fullMessage.c_str(), location);
}

void Require::Check(HRESULT hr, ErrorCode errorCode, const std::string& message, const std::source_location& location)
{
    Check(hr, errorCode, message.c_str(), location);
}