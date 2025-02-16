#include "stdafx.h"
#include "ModelException.h"

#include <sstream>

ModelException::ModelException(int line, const char* file, std::string note) noexcept 
	: BaseException(line, file), note(std::move(note))
{

}

const char* ModelException::what() const noexcept
{
	std::ostringstream out;
	out << BaseException::what() << std::endl << "[Exception] " << GetNote();

	exceptionText = out.str();

	return exceptionText.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}