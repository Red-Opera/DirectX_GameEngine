#pragma comment(lib, "Utility\\assimp\\assimp-vc143-mt.lib")

#include "stdafx.h"
#include "App.h"

#include "Core/Draw/BaseModel/ImportObjectTest.h"

#include "Test.h"
#include "Core/TestModelBase.h"

#include <iomanip>

using namespace std;

App::App(const std::string& commandLine) 
	: wnd(WINWIDTH, WINHEIGHT, "Make Box Game"), commandLine(commandLine), scriptCommander(StringConverter::TokenizeQuoted(commandLine)), light(wnd.GetDxGraphic())
{
	Test();

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-2.0f, 13.0f, -10.0f));
	//texturePlane.SetPosition({ -2.0f, 13.0f, -10.0f });
	//texturePlane.SetRotation(0.0f, 3.14f, 0.0f);
	//gobber.SetRootTransform(DirectX::XMMatrixTranslation(9.2f, 7.0f, 0.0f));
	//bluePlane.SetPosition(camera.GetPosition());
	//redPlane.SetPosition(camera.GetPosition());
	cube.SetPosition({ 4.0f, 0.0f, 0.0f });
	cube2.SetPosition({ 0.0f, 4.0f, 0.0f });

	cube.LinkTechniques(renderGraph);
	cube2.LinkTechniques(renderGraph);
	light.LinkTechniques(renderGraph);
	sponza.LinkTechniques(renderGraph);

	wnd.GetDxGraphic().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));
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
	wnd.GetDxGraphic().SetCamera(camera.GetMatrix());
	light.Update(wnd.GetDxGraphic(), camera.GetMatrix());

	light.Submit();
	//wall.Draw(wnd.GetDxGraphic());
	//texturePlane.Draw(wnd.GetDxGraphic());
	//gobber.Submit(frameCommander);
	//nano.Draw(wnd.GetDxGraphic());
	//bluePlane.Draw(wnd.GetDxGraphic());
	//redPlane.Draw(wnd.GetDxGraphic());
	cube.Submit();
	cube2.Submit();
	sponza.Submit();

	renderGraph.Execute(wnd.GetDxGraphic());

	static MB modelBase;
	modelBase.CreateWindow(sponza);

	CreateSimulationWindow();
	camera.SpawnControlWindow();
	light.CreatePositionChangeWindow();
	CreateDemoWindows();
	//wall.ShowWindow(wnd.GetDxGraphic(), "Wall");
	//texturePlane.SpawnControlWindow(wnd.GetDxGraphic());
	//gobber.ShowWindow(wnd.GetDxGraphic(), "Gobber");
	//nano.ShowWindow(wnd.GetDxGraphic(), "Nano");
	cube.SpawnControlWindow(wnd.GetDxGraphic(), "Cube 1");
	cube2.SpawnControlWindow(wnd.GetDxGraphic(), "Cube 2");
	renderGraph.RenderWidgets(wnd.GetDxGraphic());
		  
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
		}
	}

	if (!wnd.GetCursorEnabled())
	{
		if (wnd.keyBoard.IsPressed('W'))
			camera.Translate(Vector::forward * cameraDelta);

		if (wnd.keyBoard.IsPressed('A'))
			camera.Translate(Vector::left * cameraDelta);

		if (wnd.keyBoard.IsPressed('S'))
			camera.Translate(Vector::back * cameraDelta);

		if (wnd.keyBoard.IsPressed('D'))
			camera.Translate(Vector::right * cameraDelta);

		if (wnd.keyBoard.IsPressed('R'))
			camera.Translate(Vector::up * cameraDelta);

		if (wnd.keyBoard.IsPressed('F'))
			camera.Translate(Vector::down * cameraDelta);
	}

	while (const auto mouseDelta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.GetCursorEnabled())
			camera.Rotate((float)mouseDelta->x, (float)mouseDelta->y);
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