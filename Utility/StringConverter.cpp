#include "stdafx.h"
#include "StringConverter.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <filesystem>

std::vector<std::string> StringConverter::TokenizeQuoted(const std::string& input)
{
	std::istringstream stream;
	stream.str(input);

	std::vector<std::string> tokens;
	std::string token;

	while (stream >> std::quoted(token))
		tokens.push_back(token);

	return tokens;
}

std::wstring StringConverter::ToWString(const std::string& narrow)
{
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);

	if (wide[0] == L'\0')
		return L"";

	return wide;
}

std::string StringConverter::ToString(const std::wstring& wide)
{
	char narrow[512];
	wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);

	if (narrow[0] == '\0')
		return "";

	return narrow;
}

std::vector<std::string> StringConverter::SplitString(const std::string& text, const std::string& delim)
{
	std::vector<std::string> strings;
	StringConverter::SplitStringIterator(text, delim, std::back_inserter(strings));

	return strings;
}

bool StringConverter::IsStringContain(std::string_view text, std::string_view findText)
{
	return std::search(text.begin(), text.end(), findText.begin(), findText.end()) != text.end();
}

std::string StringConverter::GetAbsolutePath(const std::filesystem::path& relativePath)
{
	// 첫 번째 //까지 제거
	std::string relativePathStr = relativePath.string();

	size_t pos = relativePathStr.find("/");

	if (pos != std::string::npos)
		relativePathStr = relativePathStr.substr(pos + 1);

	return std::filesystem::absolute(relativePathStr).string();
}

std::string StringConverter::GetFileName(std::string filePath)
{
	std::filesystem::path path(filePath);
	
	return path.filename().string();
}