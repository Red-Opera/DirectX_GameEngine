#include "stdafx.h"
#include "ImguiManager.h"

#include "../imgui.h"

#include <filesystem>

ImguiManager::ImguiManager()
{
	namespace filesystem = std::filesystem;

	if (!filesystem::exists("imgui.ini") && filesystem::exists("Utility/Imgui/Usage/mgui_default.ini"))
		filesystem::copy_file("Utility/Imgui/Usage/imgui_default.ini", "imgui.ini");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
