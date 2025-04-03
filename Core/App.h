#pragma once

#include "Window.h"

#include "Camera/CameraContainer.h"
#include "Core/Scene/Scene.h"
#include "Draw/Light/PointLight.h"
#include "Object/Object.h"

#include "RenderingPipeline/RenderGraph/BlurOutlineRenderGraph.h"

#include "Utility/GameTimer.h"
#include "Utility/Imgui/ImguiManager.h"
#include "Utility/Json/ScriptCommander.h"

#include <vector>
#include <memory>

class App
{
public:
	App(const std::string& commandLine = "");
	~App();

	int Run();

	static RenderGraphNameSpace::BlurOutlineRenderGraph& GetRenderGraph();

private:
	void DoFrame(float deltaTime);
	void KeyBoardInput(float deltaTime);

	// ImGui
	void CreateSimulationWindow() noexcept;
	void CreateDemoWindows() noexcept;

	// ImGui
	ImguiManager imgui;
	bool isShowDemoWindow = false;		// ImGui의 Demo 창을 띄울 것인지 여부
	
	Window wnd;
	GameTimer timer;
	CameraContainer cameras;
	PointLight light;
	float cameraSpeed = 1.0f;

	float playSpeed = 1.0f;

	std::string commandLine;
	ScriptCommander scriptCommander;

	//TestPlane texturePlane{ wnd.GetDxGraphic(), 6.0f };

	std::shared_ptr<Scene> sponzaScene;

	bool saveDepth = false;
};