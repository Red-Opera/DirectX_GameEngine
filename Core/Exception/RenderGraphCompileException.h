#pragma once

#include "Core/Exception/BaseException.h"

#define RENDER_GRAPHIC_EXCEPTION(message) RenderGraphNameSpace::RenderGraphicCompileException((message), __LINE__, __FILE__)

namespace RenderGraphNameSpace
{
	class RenderGraphicCompileException : public BaseException
	{
	public:
		RenderGraphicCompileException(std::string message, int line, const char* file) noexcept;

#undef GetMessage
		const char* what() const noexcept override;			// 어떤 예외가 있는지 알려주는 메소드
		const char* GetType() const noexcept;				// 어떤 예외 종류인지 알려주는 메소드
		const std::string& GetMessage() const noexcept;

	private:
		std::string message;
	};
}