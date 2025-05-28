#include "stdafx.h"
#include "App.h"

#include "Camera/Camera.h"
#include "Core/Scene/EmptyScene.h"
#include "Core/Scene/SponzaScene.h"
#include "Core/Component/Physics/PhysicsSystem.h"
#include "Engine/UI/FolderView.h"
#include "Engine/UI/Inspector.h"
#include "Engine/UI/MenuBar.h"
#include "Engine/UI/ObjectGizmo.h"
#include "RenderingPipeline/RenderingChannel.h"

#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"

#include "Utility/StringConverter.h"

#include <iomanip>

using namespace std;

App::App(const std::string& commandLine) 
	: wnd(WINWIDTH, WINHEIGHT, "Sponza"), commandLine(commandLine), scriptCommander(StringConverter::TokenizeQuoted(commandLine))
{
	Engine::FolderView::GetInstance();
	Engine::MenuBar::GetInstance();
	Engine::Inspector::GetInstance();
	Engine::ObjectGizmo::GetInstance();

	// PhysX 시스템 초기화
	PhysicsSystem::GetInstance().Initialize();

	currentScene = SponzaScene::Create("Sponza");

	currentScene->Initialize();
	currentScene->BeforeFrame();
	currentScene->Start();
	currentScene->LateStart();

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-2.0f, 13.0f, -10.0f));
	//texturePlane.SetPosition({ -2.0f, 13.0f, -10.0f });
	//texturePlane.SetRotation(0.0f, 3.14f, 0.0f);
}

App::~App()
{
	// PhysX 시스템 정리
	PhysicsSystem::GetInstance().Shutdown();
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

RenderGraphNameSpace::BlurOutlineRenderGraph& App::GetRenderGraph()
{
	static RenderGraphNameSpace::BlurOutlineRenderGraph renderGraph;

	return renderGraph;
}

void App::DoFrame(float deltaTime)
{
	const float t = timer.TotalTime();
	Window::ShowGameFrame(wnd.GetHWnd());

	wnd.GetDxGraphic().BeginFrame(0.07f, 0.0f, 0.12f);

	// 물리 시스템 업데이트
	PhysicsSystem::GetInstance().Update(deltaTime);

	currentScene->Update();

	if (saveDepth)
	{
		App::GetRenderGraph().DumpShadowMap("depth.png");
		saveDepth = false;
	}

	App::GetRenderGraph().Execute();

	// auto deviceContext = wnd.GetDxGraphic().GetDeviceContext();
	// 
	// Microsoft::WRL::ComPtr<ID3D11Texture2D> dst = sceneView.GetScreen();
	// deviceContext->CopyResource(dst.Get(), wnd.GetDxGraphic().GetBackBuffer().Get());
	// 
	// Graphic::SceneView::Render();

	CreateSimulationWindow();
	CreateDemoWindows();

	currentScene->LateUpdate();

	App::GetRenderGraph().RenderWindows();

	Engine::FolderView::instance->RenderFolderView();
	Engine::MenuBar::menuBar->RenderMenuBar();
	Engine::Inspector::instance->Update();

	wnd.GetDxGraphic().EndFrame();	// 그래픽 마지막에 실행할 내용

	App::GetRenderGraph().Reset();
}

void App::KeyBoardInput(float deltaTime)
{
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
		if (wnd.keyBoard.IsPressed('R'))
			LoadScene(SponzaScene::Create("Sponza"));

		if (wnd.keyBoard.IsPressed('T'))
			LoadScene(EmptyScene::Create("EmptyScene"));
	}
	
	currentScene->CameraMoveRotation(wnd, timer, deltaTime);
}

void App::LoadScene(std::shared_ptr<Scene> scene)
{
	if (currentScene->GetName() == scene->GetName())
		return;

	currentScene = scene;
	currentScene->Initialize();
	currentScene->BeforeFrame();
	currentScene->Start();
	currentScene->LateStart();
}

void App::CreateSimulationWindow() noexcept
{
	if (ImGui::Begin("Simulation Speed", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		ImGui::SliderFloat("Speed", &playSpeed, 0.0f, 6.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("Status: %s", wnd.keyBoard.IsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}

	ImGui::End();
}

void App::CreateDemoWindows() noexcept
{
	if (isShowDemoWindow)
		ImGui::ShowDemoWindow(&isShowDemoWindow);
}