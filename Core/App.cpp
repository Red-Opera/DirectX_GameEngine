#pragma comment(lib, "Utility\\assimp\\assimp-vc143-mt.lib")

#include "stdafx.h"
#include "App.h"

#include "Core/TestModelBase.h"
#include "Core/Camera/Camera.h"
#include "Core/EngineUI/FolderViewInspector.h"
#include "Core/EngineUI/MenuBar.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/Test.h"

#include <iomanip>

using namespace std;

App::App(const std::string& commandLine) 
	: wnd(WINWIDTH, WINHEIGHT, "Sponza"), commandLine(commandLine), scriptCommander(StringConverter::TokenizeQuoted(commandLine)), 
	  light({ 0.0f, 10.0f, 0.0f })
{
	Engine::FolderViewInspector::GetInstance();
	Engine::MenuBar::GetInstance();

	cameras.AddCamera(std::make_unique<Camera>("A", DirectX::XMFLOAT3{ -22.0f, 4.0f, 0.0f }, 0.0f, Math::PI / 2.0f));
	cameras.AddCamera(std::make_unique<Camera>("B", DirectX::XMFLOAT3{ -13.5f,28.8f,-6.4f }, Math::PI / 180.0f * 13.0f, Math::PI / 180.0f * 61.0f));
	cameras.AddCamera(light.GetLightViewCamera());

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-2.0f, 13.0f, -10.0f));
	//texturePlane.SetPosition({ -2.0f, 13.0f, -10.0f });
	//texturePlane.SetRotation(0.0f, 3.14f, 0.0f);
	//gobber.SetRootTransform(DirectX::XMMatrixTranslation(9.2f, 7.0f, 0.0f));
	//bluePlane.SetPosition(camera.GetPosition());
	//redPlane.SetPosition(camera.GetPosition());
	cube.SetPosition({ 14.0f, 0.0f, 0.0f });
	cube2.SetPosition({ 10.0f, 4.0f, 0.0f });
	colorCube.SetPosition({ 4.0f, 2.0f, 0.0f });
	colorCube2.SetPosition({ 0.0f, 6.0f, 0.0f });
	colorSphere.SetPosition({ -4.0f, 10.0f, 0.0f });
	colorSphere.SetRotation({ 0.0f, 0.0f, 0.0f });

	nano.SetRootTransform(
		DirectX::XMMatrixRotationY(Math::PI / 2.f) *
		DirectX::XMMatrixTranslation(27.f, -0.56f, 1.7f)
	);
	gobber.SetRootTransform(
		DirectX::XMMatrixRotationY(-Math::PI / 2.f) *
		DirectX::XMMatrixTranslation(-30.f, 10.f, 0.f)
	);

	cube.LinkTechniques(renderGraph);
	cube2.LinkTechniques(renderGraph);
	colorCube.LinkTechniques(renderGraph);
	colorCube2.LinkTechniques(renderGraph);
	colorSphere.LinkTechniques(renderGraph);
	colorSphere2.LinkTechniques(renderGraph);
	light.LinkTechniques(renderGraph);
	sponza.LinkTechniques(renderGraph);
	gobber.LinkTechniques(renderGraph);
	nano.LinkTechniques(renderGraph);
	cameras.LinkTechniques(renderGraph);

	renderGraph.RenderShadowCamera(*light.GetLightViewCamera());
}

App::~App()
{
}

int App::Run()
{
	timer = GameTimer();
	timer.Reset();
	timer.Start();

	while (true)
	{
		if (const auto encode = Window::ProcessMessages())
			return *encode;

		const float deltaTime = timer.DeltaTime();
		KeyBoardInput(deltaTime);
		DoFrame(deltaTime);

		timer.Tick();
	}
}

void App::DoFrame(float deltaTime)
{
	const float t = timer.TotalTime();
	Window::ShowGameFrame(wnd.GetHWnd());

	wnd.GetDxGraphic().BeginFrame(0.07f, 0.0f, 0.12f);

	light.Update(cameras->GetMatrix());
	renderGraph.RenderMainCamera(cameras.GetActiveCamera());

	light.Submit(RenderingChannel::main);
	cube.Submit(RenderingChannel::main);
	cube2.Submit(RenderingChannel::main);
	colorCube.Submit(RenderingChannel::main);
	colorCube2.Submit(RenderingChannel::main);
	colorSphere.Submit(RenderingChannel::main);
	colorSphere2.Submit(RenderingChannel::main);
	sponza.Submit(RenderingChannel::main);
	nano.Submit(RenderingChannel::main);
	gobber.Submit(RenderingChannel::main);
	cameras.Submit(RenderingChannel::main);

	sponza.Submit(RenderingChannel::shadow);
	cube.Submit(RenderingChannel::shadow);
	cube2.Submit(RenderingChannel::shadow);
	colorCube.Submit(RenderingChannel::shadow);
	colorCube2.Submit(RenderingChannel::shadow);
	colorSphere.Submit(RenderingChannel::shadow);
	colorSphere2.Submit(RenderingChannel::shadow);
	gobber.Submit(RenderingChannel::shadow);
	nano.Submit(RenderingChannel::shadow);

	if (saveDepth)
	{
		renderGraph.DumpShadowMap("depth.png");
		saveDepth = false;
	}

	renderGraph.Execute();

	// auto deviceContext = wnd.GetDxGraphic().GetDeviceContext();
	// 
	// Microsoft::WRL::ComPtr<ID3D11Texture2D> dst = sceneView.GetScreen();
	// deviceContext->CopyResource(dst.Get(), wnd.GetDxGraphic().GetBackBuffer().Get());
	// 
	// Graphic::SceneView::Render();

	static MB sponzaBase{ "Sponza" };
	static MB gobbarBase{ "Gobbar" };
	static MB nanoBase{ "Nano" };
	sponzaBase.CreateWindow(sponza);
	gobbarBase.CreateWindow(gobber);
	nanoBase.CreateWindow(nano);

	CreateSimulationWindow();
	cameras.CreateWindow();
	light.CreatePositionChangeWindow();
	CreateDemoWindows();
	//wall.ShowWindow(wnd.GetDxGraphic(), "Wall");
	//texturePlane.SpawnControlWindow(wnd.GetDxGraphic());
	//gobber.ShowWindow(wnd.GetDxGraphic(), "Gobber");
	//nano.ShowWindow(wnd.GetDxGraphic(), "Nano");
	cube.SpawnControlWindow("Cube 1");
	cube2.SpawnControlWindow("Cube 2");
	colorCube.CreateControlWindow("Color Cube");
	colorCube2.CreateControlWindow("Color Cube 2");
	colorSphere.CreateControlWindow("Color Sphere");
	colorSphere2.CreateControlWindow("Color Sphere 2");
	renderGraph.RenderWindows();

	Engine::FolderViewInspector::instance->RenderFolderView();
	Engine::FolderViewInspector::instance->RenderInspector();
	Engine::MenuBar::menuBar->RenderMenuBar();

	wnd.GetDxGraphic().EndFrame();	// 그래픽 마지막에 실행할 내용

	renderGraph.Reset();
}

void App::KeyBoardInput(float deltaTime)
{
	auto cameraDelta = timer.DeltaTime() * cameraSpeed;

	while (const auto currentKey = wnd.keyBoard.ReadKey())
	{
		if (!currentKey->IsPress())
			continue;

		switch (currentKey->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.GetCursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}

			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}

			break;

		case VK_F1:
			isShowDemoWindow = true;
			break;
			
		case VK_RETURN:
			saveDepth = true;
			break;
		}
	}

	if (!wnd.GetCursorEnabled())
	{
		if (wnd.keyBoard.IsPressed(VK_SHIFT))
			cameraDelta *= 5.0f;

		if (wnd.keyBoard.IsPressed('W'))
			cameras->Translate(Vector::forward * cameraDelta);

		if (wnd.keyBoard.IsPressed('A'))
			cameras->Translate(Vector::left * cameraDelta);

		if (wnd.keyBoard.IsPressed('S'))
			cameras->Translate(Vector::back * cameraDelta);

		if (wnd.keyBoard.IsPressed('D'))
			cameras->Translate(Vector::right * cameraDelta);

		if (wnd.keyBoard.IsPressed('Q'))
			cameras->Translate(Vector::up * cameraDelta);

		if (wnd.keyBoard.IsPressed('E'))
			cameras->Translate(Vector::down * cameraDelta);
	}

	while (const auto mouseDelta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.GetCursorEnabled())
			cameras->Rotate((float)mouseDelta->x, (float)mouseDelta->y);
	}
}

void App::CreateSimulationWindow() noexcept
{
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed", &objectSpeed, 0.0f, 6.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("Status: %s", wnd.keyBoard.IsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}
	ImGui::End();
}

void App::CreateDemoWindows() noexcept
{
	if (isShowDemoWindow)
		ImGui::ShowDemoWindow(&isShowDemoWindow);
}