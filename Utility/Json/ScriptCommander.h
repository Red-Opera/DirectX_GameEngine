#pragma once

#include "Core/Exception/BaseException.h"

#include <string>
#include <vector>

class ScriptCommander
{
public:
	class Completion : public BaseException
	{
	public:
		Completion(const std::string& content) noexcept;

		const char* what() const noexcept override;
		const char* GetType() const noexcept override;

	private:
		std::string content;
	};

	class Exception : public BaseException
	{
	public:
		Exception(int line, const char* file, const std::string& script = "", const std::string& message = "") noexcept;

		const char* what() const noexcept override;
		const char* GetType() const noexcept override;

	private:
		std::string script;
		std::string message;
	};

	ScriptCommander(const std::vector<std::string>& args);
};