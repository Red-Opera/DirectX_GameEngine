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
		const std::string& message,
		const std::source_location& location =
		std::source_location::current());

	// DXGI 및 D3D11 디버그 메시지 검사 및 에러 처리
	template<typename Func>
	static void Check(Func&& func, ErrorCode errorCode, const std::string& message, const std::source_location location = std::source_location::current());

private:
	Require();
	~Require() = default;
};

template<typename Func>
inline void Require::Check(Func&& func, ErrorCode errorCode, const std::string& message, const std::source_location location)
{
	ExceptionInfo& infoManager = ExceptionInfo::GetCurrent();

	infoManager.Set();

	std::forward<Func>(func)();

	const char* errorMessage = infoManager.GetMessages();

	if (errorMessage == nullptr || strlen(errorMessage) == 0)
		return;

	Check(false, errorCode, errorMessage, location);
}