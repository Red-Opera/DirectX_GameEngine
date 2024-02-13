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
	string exContent = "���ܰ� �߻��߽��ϴ�. (���� ���� : " +
		string(GetType()) + ", ���� ���� : " +
		file + "�� ��ȣ : " + to_string(line) + "��)";		

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
	out << "[���� �̸�] " << file << endl << "[�� ��ȣ] " << line;

	return out.str();
}
