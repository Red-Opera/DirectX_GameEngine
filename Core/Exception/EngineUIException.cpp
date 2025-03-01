#include "stdafx.h"
#include "EngineUIException.h"

EngineUIException::EngineUIException(std::string message, int line, const char* file) noexcept
	: BaseException(line, file), message(std::move(message))
{

}

const char* EngineUIException::what() const noexcept
{
	std::ostringstream output;
	output << BaseException::what() << std::endl;
	output << "[Message]" << std::endl;
	output << message;

	exceptionText = output.str();

	return exceptionText.c_str();
}

const char* EngineUIException::GetType() const noexcept
{
	return "엔진 UI 런타임 예외";
}

const std::string& EngineUIException::GetMessage() const noexcept
{
	return message;
}