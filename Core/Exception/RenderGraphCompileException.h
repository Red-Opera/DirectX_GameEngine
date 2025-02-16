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
		const char* what() const noexcept override;			// � ���ܰ� �ִ��� �˷��ִ� �޼ҵ�
		const char* GetType() const noexcept;				// � ���� �������� �˷��ִ� �޼ҵ�
		const std::string& GetMessage() const noexcept;

	private:
		std::string message;
	};
}