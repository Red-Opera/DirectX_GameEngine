#include "stdafx.h"
#include "ScriptCommander.h"

#include "Core/Draw/Base/Image/TexturePreprocessor.h"
#include "Utility/Json/json.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Json = nlohmann;
using namespace std::string_literals;

#define SCRIPT_ERROR(message) Exception(__LINE__, __FILE__, scriptPath, (message))

ScriptCommander::Completion::Completion(const std::string& content) noexcept
	: BaseException(69, "@ScriptCommanderAbort"), content(content)
{

}

const char* ScriptCommander::Completion::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << std::endl << content;
	exceptionText = oss.str();

	return exceptionText.c_str();
}

const char* ScriptCommander::Completion::GetType() const noexcept
{
	return "Script Command Completed";
}

ScriptCommander::Exception::Exception(int line, const char* file, const std::string& script, const std::string& message) noexcept
	: BaseException(69, "@ScriptCommanderAbort"), script(script), message(message)
{

}

const char* ScriptCommander::Exception::what() const noexcept
{
	std::ostringstream out;

	out << BaseException::what() << std::endl << "[Script File] " << script << std::endl << message;
	exceptionText = out.str();

	return exceptionText.c_str();
}

const char* ScriptCommander::Exception::GetType() const noexcept
{
	return "Script Command Error";
}

ScriptCommander::ScriptCommander(const std::vector<std::string>& args)
{
	if (args.size() > 2 && args[0] == "--commands")
	{
		const auto scriptPath = args[1];
		std::ifstream script(scriptPath);

		if (!script.is_open())
			throw Exception(__LINE__, __FILE__, scriptPath, "파일을 열 수 없음");

		Json::json json;
		script >> json;

		if (!json.at("enabled"))
		{
			bool abort = false;

			for (const auto& j : json.at("commands"))
			{
				const auto commandName = j.at("command").get<std::string>();
				const auto params = j.at("params");

				if (commandName == "flip-y")
				{
					const auto source = params.at("source");

					TexturePreprocessor::FilpYNormalMap(source, params.value("dest", source));
					abort = true;
				}

				else if (commandName == "flip-y-obj")
				{
					TexturePreprocessor::FilpYAllNormalMapsInObject(params.at("source"));
					abort = true;
				}

				else if (commandName == "validate-nmap")
				{
					TexturePreprocessor::VaildateNormalMap(params.at("source"), params.at("min"), params.at("max"));
					abort = true;
				}

				else if (commandName == "make-stripes")
				{
					TexturePreprocessor::MakeStripes(params.at("dest"), params.at("size"), params.at("stripeWidth"));
					abort = true;
				}

				else if (commandName == "Export")
				{
					Export(params.at("dest"));
					abort = true;
				}

				else
					throw SCRIPT_ERROR("알 수 없는 명령어 : "s + commandName);
			}

			if (abort)
				throw Completion("스크립트 실행 완료");
		}
	}
}

void ScriptCommander::Export(std::string path) const
{
	namespace fs = std::filesystem;

	fs::create_directory(path);
	fs::copy_file(R"(..\x64\Release\GameEngine.exe)", path + R"(\GameEngine.exe)", fs::copy_options::overwrite_existing);
	fs::copy_file("Utility/Imgui/Usage/imgui_default.ini", path + R"(Utility/Imgui/Usage/imgui_default.ini)", fs::copy_options::overwrite_existing);

	for (auto& p : fs::directory_iterator(""))
	{
		if (p.path().extension() == L".dll")
			fs::copy_file(p.path(), path + "\\" + p.path().filename().string(), fs::copy_options::overwrite_existing);
	}

	fs::copy("ShaderBins", path + R"(\ShaderBins)", fs::copy_options::overwrite_existing);
	fs::copy("Images", path + R"(\Images)", fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	fs::copy("Models", path + R"(\Models)", fs::copy_options::overwrite_existing | fs::copy_options::recursive);
}