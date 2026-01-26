#pragma once

#include "Core/Exception/ExceptionInfo.h"
#include "Core/Exception/Base/ErrorCode.h"

#include <Windows.h>

#include <source_location>

// 전방 선언
class ExceptionInfo;
class DxGraphic;

class Require
{
public:
	// 복사 및 이동 방지
	Require(const Require&) = delete;
	Require& operator=(const Require&) = delete;
	Require(Require&&) = delete;
	Require& operator=(Require&&) = delete;

	// 에러 처리
	static void Check(bool isTrue, ErrorCode errorCode,
		const char* message = nullptr,
		const std::source_location& location =
		std::source_location::current());

	// HRESULT 실패 검사 및 에러 처리
	static void Check(HRESULT hr,
		ErrorCode errorCode,
		const char* message = nullptr,
		const std::source_location& location =
		std::source_location::current());

	// HRESULT 실패 검사 및 에러 처리 (std::string 버전)
	static void Check(HRESULT hr,
		ErrorCode errorCode,
		const std::string_view message,
		const std::source_location& location =
		std::source_location::current());

	// DXGI 및 D3D11 디버그 메시지 검사 및 에러 처리
	template<typename Func>
	__forceinline static void Check(Func&& func, ErrorCode errorCode, const std::string_view message, const std::source_location& location = std::source_location::current());

private:
	Require();
	~Require() = default;
};

template<typename Func>
__forceinline void Require::Check(Func&& func, ErrorCode errorCode, const std::string_view message, const std::source_location& location)
{
#ifndef NDEBUG
	ExceptionInfo& infoManager = ExceptionInfo::GetCurrent();

	// DXGI 디버그를 사용하지 않으면 종료
	if (ExceptionInfo::useDXGIDebug)
		infoManager.Set();

	std::forward<Func>(func)();

	if (ExceptionInfo::useDXGIDebug)
	{
		const char* errorMessage = infoManager.GetMessages();

		if (errorMessage != nullptr && errorMessage[0] != '\0')
			Check(false, errorCode, errorMessage, location);
	}
#else
	std::forward<Func>(func)(); // Release 모드에서는 함수만 실행
#endif
}