#pragma once

#include "BaseException.h"

#define ENGINE_UI_EXCEPTION(message) EngineUIException((message), __LINE__, __FILE__)

class EngineUIException : public BaseException
{
public:
	EngineUIException(std::string message, int line, const char* file) noexcept;

#undef GetMessage
	const char* what() const noexcept override;			// 어떤 예외가 있는지 알려주는 메소드
	const char* GetType() const noexcept;				// 어떤 예외 종류인지 알려주는 메소드
	const std::string& GetMessage() const noexcept;

private:
	std::string message;
};