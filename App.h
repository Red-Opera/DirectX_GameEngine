#pragma once

#include <set>

#include "Core/Window.h"
#include "Core/Camera.h"
#include "Core/Draw/Light/PointLight.h"

#include "Utility/Imgui/Usage/ImguiManager.h"

class App
{
public:
	App();
	~App();

	int Run();

private:
	void DoFrame();

	// ImGui
	void CreateSimulationWindow() noexcept;
	void CreateBoxWindowManagerWindow() noexcept;
	void CreateBoxWindows() noexcept;

	// ImGui
	ImguiManager imgui;
	bool show_demo_window = true;		// ImGui의 Demo 창을 띄울 것인지 여부
	
	Window wnd;
	GameTimer timer;
	Camera camera;
	PointLight light;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class DrawBox*> boxes;
	float objectSpeed = 1.0f;
	static constexpr size_t drawableCount = 180;

	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};

