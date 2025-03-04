#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <filesystem>

class StringConverter
{
public:
	static std::vector<std::string> TokenizeQuoted(const std::string& input);

	static std::wstring ToWide(const std::string& narrow);
	static std::string ToNarrow(const std::wstring& wide);

	template<class Iter>
	static void SplitStringIterator(const std::string& text, const std::string& delim, Iter out)
	{
		if (delim.empty())
			*out++ = text;

		else
		{
			size_t a = 0, b = text.find(delim);

			for (; b != std::string::npos; a = b + delim.length(), b = text.find(delim, a))
				*out++ = std::move(text.substr(a, b - a));

			*out++ = std::move(text.substr(a, text.length() - a));
		}
	}

	static std::vector<std::string> SplitString(const std::string& text, const std::string& delim);
	static bool IsStringContain(std::string_view text, std::string_view findText);

	static std::string GetAbsolutePath(const std::filesystem::path& path);
	static std::string GetFileName(std::string filePath);
	static std::wstring ToWString(const std::string& value);
	static std::string ToString(const std::wstring& value);
};