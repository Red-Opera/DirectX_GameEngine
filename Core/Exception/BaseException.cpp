#include "stdafx.h"
#include "BaseException.h"
using namespace std;

BaseException::BaseException(int line, const char* file) noexcept
	: line(line), file(file)
{

}

BaseException::BaseException(int line, string file) noexcept
	: line(line), file(file)
{
}

const char* BaseException::what() const noexcept
{
	string exContent = "예외가 발생했습니다. (예외 종류 : " +
		string(GetType()) + ", 예외 파일 : " +
		file + "줄 번호 : " + to_string(line) + "줄)";		

	exceptionText = exContent;

	return exceptionText.c_str();
}

const char* BaseException::GetType() const noexcept
{
	return "BaseException";
}

int BaseException::GetLine() const noexcept
{
	return line;
}

const std::string& BaseException::GetFile() const noexcept
{
	return file;
}

string BaseException::GetFileNameAndLine() const noexcept
{
	ostringstream out;
	out << "[파일 이름] " << file << endl << "[줄 번호] " << line;

	return out.str();
}
