#include "stdafx.h"
#include "ImguiManager.h"

#include "External/Imgui/imgui.h"

#include <filesystem>

ImguiManager::ImguiManager()
{
	namespace filesystem = std::filesystem;

	if (!filesystem::exists("Utility/Imgui/imgui.ini") && filesystem::exists("Utility/Imgui/imgui_default.ini"))
		filesystem::copy_file("Utility/Imgui/imgui_default.ini", "Utility/Imgui/imgui.ini");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
