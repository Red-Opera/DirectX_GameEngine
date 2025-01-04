#pragma once

#include <set>

#include "Core/Window.h"
#include "Core/Camera.h"
#include "Core/Draw/Light/PointLight.h"
#include "Core/Draw/Mesh.h"
#include "Core/Draw/TestPlane.h"

#include "Utility/Imgui/Usage/ImguiManager.h"

class App
{
public:
	App(const std::string& commandLine = "");
	~App();

	int Run();

private:
	void DoFrame();

	// ImGui
	void CreateSimulationWindow() noexcept;
	void CreateBoxWindowManagerWindow() noexcept;
	void CreateDemoWindows() noexcept;

	// ImGui
	ImguiManager imgui;
	bool isShowDemoWindow = false;		// ImGui의 Demo 창을 띄울 것인지 여부
	
	Window wnd;
	GameTimer timer;
	Camera camera;
	PointLight light;
	float cameraSpeed = 1.0f;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class DrawBox*> boxes;
	float objectSpeed = 1.0f;
	static constexpr size_t drawableCount = 180;

	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;

	std::string commandLine;

	//Model wall { wnd.GetDxGraphic(), "Model/Sample/brick_wall/brick_wall.obj", 6.0f };
	//Model gobber{ wnd.GetDxGraphic(),"Model/Sample/gobber/GoblinX.obj", 6.0f };
	//Model nano { wnd.GetDxGraphic(),"Model/Sample/nano_textured/nanosuit.obj", 2.0f };
	Model sponza { wnd.GetDxGraphic(),"Model/Sample/sponza/sponza.obj", 1.0f / 20.0f };
	//TestPlane texturePlane{ wnd.GetDxGraphic(), 6.0f };
	TestPlane bluePlane{ wnd.GetDxGraphic(), 6.0f, {0.3f, 0.3f, 1.0f, 0.0f} };
	TestPlane redPlane{ wnd.GetDxGraphic(), 6.0f, {1.0f, 0.3f, 0.3f, 0.0f} };
};

