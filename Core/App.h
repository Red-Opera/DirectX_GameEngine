#pragma once

#include "Window.h"
#include "Camera/CameraContainer.h"
#include "Draw/Light/PointLight.h"
#include "Draw/Model.h"
#include "Draw/Object/TextureCubeObject.h"
#include "Draw/Object/ColorCubeObject.h"
#include "Draw/Object/ColorSphereObject.h"

#include "RenderingPipeline/RenderGraph/BlurOutlineRenderGraph.h"
#include "EngineUI/FolderViewInspector.h"

#include "Utility/GameTimer.h"
#include "Utility/Imgui/ImguiManager.h"
#include "Utility/Json/ScriptCommander.h"
#include "Utility/MathInfo.h"

class App
{
public:
	App(const std::string& commandLine = "");
	~App();

	int Run();

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

	std::vector<std::unique_ptr<class Drawable>> drawables;
	float objectSpeed = 1.0f;

	std::string commandLine;
	ScriptCommander scriptCommander;
	RenderGraphNameSpace::BlurOutlineRenderGraph renderGraph{ };

	//Model wall { wnd.GetDxGraphic(), "Model/Sample/brick_wall/brick_wall.obj", 6.0f };
	Model gobber{ "Model/Sample/gobber/GoblinX.obj", 4.0f };
	Model nano { "Model/Sample/nano_textured/nanosuit.obj", 1.0f };
	Model sponza { "Model/Sample/sponza/sponza.obj", 1.0f / 20.0f };
	//TestPlane texturePlane{ wnd.GetDxGraphic(), 6.0f };
	//TestPlane bluePlane{ wnd.GetDxGraphic(), 6.0f, {0.3f, 0.3f, 1.0f, 0.0f} };
	//TestPlane redPlane{ wnd.GetDxGraphic(), 6.0f, {1.0f, 0.3f, 0.3f, 0.0f} };

	TextureCubeObject cube{ 4.0f, "Images/brickwall.jpg" };
	TextureCubeObject cube2{ 4.0f, "Images/brickwall.jpg" };

	ColorCubeObject colorCube{ 4.0f, { 255, 0, 0 } };
	ColorCubeObject colorCube2{ 4.0f, { 255, 0, 0 }, false };

	ColorSphereObject colorSphere{ 1.0f, { 0, 255, 0 } };
	ColorSphereObject colorSphere2{ 1.0f, { 0, 255, 0 }, false };

	bool saveDepth = false;
};

