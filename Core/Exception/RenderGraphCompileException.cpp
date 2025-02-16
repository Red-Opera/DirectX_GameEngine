#include "stdafx.h"
#include "RenderGraphCompileException.h"

#include <sstream>

namespace RenderGraphNameSpace
{
	RenderGraphicCompileException::RenderGraphicCompileException(std::string message, int line, const char* file) noexcept
		: BaseException(line, file), message(std::move(message))
	{

	}

	const char* RenderGraphicCompileException::what() const noexcept
	{
		std::ostringstream output;
		output << BaseException::what() << std::endl;
		output << "[Message]" << std::endl;
		output << message;

		exceptionText = output.str();

		return exceptionText.c_str();
	}

	const char* RenderGraphicCompileException::GetType() const noexcept
	{
		return "Render Graphic 컴파일 예외";
	}

	const std::string& RenderGraphicCompileException::GetMessage() const noexcept
	{
		return message;
	}
}