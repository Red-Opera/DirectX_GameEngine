#pragma comment(lib, "Utility\\assimp\\assimp-vc143-mt.lib")

#include "stdafx.h"
#include "App.h"

#include "Core/TestModelBase.h"
#include "Core/Camera/Camera.h"
#include "Core/EngineUI/EngineUI.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/Test.h"

#include <iomanip>

using namespace std;

App::App(const std::string& commandLine) 
	: wnd(WINWIDTH, WINHEIGHT, "Make Box Game"), commandLine(commandLine), scriptCommander(StringConverter::TokenizeQuoted(commandLine)), 
	  light(wnd.GetDxGraphic(), { 0.0f, 10.0f, 0.0f })
{
	Engine::EngineUI;

	cameras.AddCamera(std::make_unique<Camera>(wnd.GetDxGraphic(), "A", DirectX::XMFLOAT3{ -22.0f, 4.0f, 0.0f }, 0.0f, Math::PI / 2.0f));
	cameras.AddCamera(std::make_unique<Camera>(wnd.GetDxGraphic(), "B", DirectX::XMFLOAT3{ -13.5f,28.8f,-6.4f }, Math::PI / 180.0f * 13.0f, Math::PI / 180.0f * 61.0f));
	cameras.AddCamera(light.GetLightViewCamera());

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-2.0f, 13.0f, -10.0f));
	//texturePlane.SetPosition({ -2.0f, 13.0f, -10.0f });
	//texturePlane.SetRotation(0.0f, 3.14f, 0.0f);
	//gobber.SetRootTransform(DirectX::XMMatrixTranslation(9.2f, 7.0f, 0.0f));
	//bluePlane.SetPosition(camera.GetPosition());
	//redPlane.SetPosition(camera.GetPosition());
	cube.SetPosition({ 4.0f, 0.0f, 0.0f });
	cube2.SetPosition({ 0.0f, 4.0f, 0.0f });

	nano.SetRootTransform(
		DirectX::XMMatrixRotationY(Math::PI / 2.f) *
		DirectX::XMMatrixTranslation(27.f, -0.56f, 1.7f)
	);
	gobber.SetRootTransform(
		DirectX::XMMatrixRotationY(-Math::PI / 2.f) *
		DirectX::XMMatrixTranslation(-8.f, 10.f, 0.f)
	);

	cube.LinkTechniques(renderGraph);
	cube2.LinkTechniques(renderGraph);
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

	ostringstream out;
	out << "Play Time : " << setprecision(1) << fixed << t << "s";
	wnd.GetDxGraphic().BeginFrame(0.07f, 0.0f, 0.12f);

	light.Update(wnd.GetDxGraphic(), cameras->GetMatrix());
	renderGraph.RenderMainCamera(cameras.GetActiveCamera());

	light.Submit(RenderingChannel::main);
	cube.Submit(RenderingChannel::main);
	cube2.Submit(RenderingChannel::main);
	sponza.Submit(RenderingChannel::main);
	nano.Submit(RenderingChannel::main);
	gobber.Submit(RenderingChannel::main);
	cameras.Submit(RenderingChannel::main);

	sponza.Submit(RenderingChannel::shadow);
	cube.Submit(RenderingChannel::shadow);
	cube2.Submit(RenderingChannel::shadow);
	gobber.Submit(RenderingChannel::shadow);
	nano.Submit(RenderingChannel::shadow);

	renderGraph.Execute(wnd.GetDxGraphic());

	if (saveDepth)
	{
		renderGraph.DumpShadowMap(wnd.GetDxGraphic(), "depth.png");
		saveDepth = false;
	}

	static MB sponzaBase{ "Sponza" };
	static MB gobbarBase{ "Gobbar" };
	static MB nanoBase{ "Nano" };
	sponzaBase.CreateWindow(sponza);
	gobbarBase.CreateWindow(gobber);
	nanoBase.CreateWindow(nano);

	CreateSimulationWindow();
	cameras.CreateWindow(wnd.GetDxGraphic());
	light.CreatePositionChangeWindow();
	CreateDemoWindows();
	//wall.ShowWindow(wnd.GetDxGraphic(), "Wall");
	//texturePlane.SpawnControlWindow(wnd.GetDxGraphic());
	//gobber.ShowWindow(wnd.GetDxGraphic(), "Gobber");
	//nano.ShowWindow(wnd.GetDxGraphic(), "Nano");
	cube.SpawnControlWindow(wnd.GetDxGraphic(), "Cube 1");
	cube2.SpawnControlWindow(wnd.GetDxGraphic(), "Cube 2");
	renderGraph.RenderWindows(wnd.GetDxGraphic());

	Engine::EngineUI::instance->RenderFolderView();
	Engine::EngineUI::instance->RenderInspector();
		  
	wnd.SetTitle(out.str());		// 제목 동기화
	wnd.GetDxGraphic().EndFrame();	// 그래픽 마지막에 실행할 내용

	renderGraph.Reset();
}

void App::KeyBoardInput(float deltaTime)
{
	const auto cameraDelta = timer.DeltaTime() * cameraSpeed;

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
		if (wnd.keyBoard.IsPressed('W'))
			cameras->Translate(Vector::forward * cameraDelta);

		if (wnd.keyBoard.IsPressed('A'))
			cameras->Translate(Vector::left * cameraDelta);

		if (wnd.keyBoard.IsPressed('S'))
			cameras->Translate(Vector::back * cameraDelta);

		if (wnd.keyBoard.IsPressed('D'))
			cameras->Translate(Vector::right * cameraDelta);

		if (wnd.keyBoard.IsPressed('R'))
			cameras->Translate(Vector::up * cameraDelta);

		if (wnd.keyBoard.IsPressed('F'))
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
		ImGui::SliderFloat("Speed", &objectSpeed, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.keyBoard.IsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}
	ImGui::End();
}

void App::CreateDemoWindows() noexcept
{
	if (isShowDemoWindow)
		ImGui::ShowDemoWindow(&isShowDemoWindow);
}