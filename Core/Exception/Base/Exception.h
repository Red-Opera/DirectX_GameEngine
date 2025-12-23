#pragma once

class Exception
{
public:
	// 복사 및 이동 방지
	Exception(const Exception&) = delete;
	Exception& operator=(const Exception&) = delete;
	Exception(Exception&&) = delete;
	Exception& operator=(Exception&&) = delete;

	static void Error(const ErrorCode&& errorCode, const char* message = nullptr);

private:
	Exception();
	~Exception() = default;
};