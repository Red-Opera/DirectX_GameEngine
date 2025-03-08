#include "stdafx.h"
#include "MenuBar.h"

#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx11.h"
#include "External/Imgui/imgui_impl_win32.h"

std::unique_ptr<Engine::MenuBar> Engine::MenuBar::menuBar;

namespace Engine
{
	void MenuBar::RenderMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open"))
				{

				}

				if (ImGui::MenuItem("Save"))
				{

				}

				if (ImGui::MenuItem("Exit"))
					PostQuitMessage(0);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo"))
				{

				}

				if (ImGui::MenuItem("Redo"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About")) 
				{ 
				
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}