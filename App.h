#pragma once

#include "Core/Window.h"
#include "Core/Camera.h"

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
	ImguiManager imgui;
	bool show_demo_window = true;		// ImGui�� Demo â�� ��� ������ ����
	
	Window wnd;
	GameTimer timer;
	Camera camera;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	float objectSpeed = 1.0f;
	static constexpr size_t drawableCount = 180;
};

