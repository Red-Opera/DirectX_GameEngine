#pragma once

#include <source_location>
#include <Windows.h>

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

	static void Check(HRESULT hr,
		ErrorCode errorCode,
		const std::string& message,
		const std::source_location& location =
		std::source_location::current());

private:
	Require();
	~Require() = default;
};