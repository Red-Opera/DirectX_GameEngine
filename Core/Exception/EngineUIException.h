#pragma once

#include "BaseException.h"

#define ENGINE_UI_EXCEPTION(message) EngineUIException((message), __LINE__, __FILE__)

class EngineUIException : public BaseException
{
public:
	EngineUIException(std::string message, int line, const char* file) noexcept;

#undef GetMessage
	const char* what() const noexcept override;			// � ���ܰ� �ִ��� �˷��ִ� �޼ҵ�
	const char* GetType() const noexcept;				// � ���� �������� �˷��ִ� �޼ҵ�
	const std::string& GetMessage() const noexcept;

private:
	std::string message;
};