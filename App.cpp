#pragma comment(lib, "Utility\\assimp\\assimp-vc143-mt.lib")

//#define FULL_WINTARD
#include "stdafx.h"
#include "App.h"

#include "Core/Draw/BaseModel/ImportObjectTest.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Draw/Base/Image/GDIPlus.h"

#include "Core/RenderingPipeline/IA/VertexBuffer.h"

#include <iomanip>
using namespace std;

GDIPlus gdiPlus;

App::App() : wnd(WINWIDTH, WINHEIGHT, "Make Box Game"), light(wnd.GetDxGraphic())
{
	wnd.GetDxGraphic().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));
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

		DoFrame();
		timer.Tick();
	}
}

void App::DoFrame()
{
	const float t = timer.TotalTime();
	const float deltaTime = timer.DeltaTime();
	const auto cameraDelta = timer.DeltaTime() * cameraSpeed;

	ostringstream out;
	out << "Play Time : " << setprecision(1) << fixed << t << "s";
	wnd.GetDxGraphic().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.GetDxGraphic().SetCamera(camera.GetMatrix());
	light.Bind(wnd.GetDxGraphic(), camera.GetMatrix());

	// 모든 기하구조를 업데이트한 후 그림
	//for (auto& box : drawables)
	//{
	//	box->Update(wnd.keyBoard.IsPressed(VK_SPACE) ? 0.0f : deltaTime * objectSpeed);
	//	box->Draw(wnd.GetDxGraphic());
	//}

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

	model.Draw(wnd.GetDxGraphic());
	model2.Draw(wnd.GetDxGraphic());
	light.Draw(wnd.GetDxGraphic());

	CreateSimulationWindow();
	camera.SpawnControlWindow();
	light.CreatePositionChangeWindow();
	CreateDemoWindows();
	model.ShowWindow("Model 1");
	model2.ShowWindow("Model 2");

	// imgui window to open box windows
	//CreateBoxWindowManagerWindow();

	// 박스 상태 창 생성
	//CreateBoxWindows();
	
	wnd.SetTitle(out.str());		// 제목 동기화
	wnd.GetDxGraphic().EndFrame();	// 그래픽 마지막에 실행할 내용
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

void App::CreateBoxWindowManagerWindow() noexcept
{
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;

		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				bool selected = false;

				if (comboBoxIndex != nullopt)
					selected = *comboBoxIndex == i;

				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
					comboBoxIndex = i;

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::CreateDemoWindows() noexcept
{
	if (isShowDemoWindow)
		ImGui::ShowDemoWindow(&isShowDemoWindow);
}