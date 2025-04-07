#include "stdafx.h"
#include "App.h"

#include "Camera/Camera.h"
#include "Core/Scene/EmptyScene.h"
#include "Core/Scene/SponzaScene.h"
#include "EngineUI/FolderView.h"
#include "EngineUI/Inspector.h"
#include "EngineUI/MenuBar.h"
#include "RenderingPipeline/RenderingChannel.h"

#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"

#include "Utility/StringConverter.h"

#include <iomanip>

using namespace std;

App::App(const std::string& commandLine) 
	: wnd(WINWIDTH, WINHEIGHT, "Sponza"), commandLine(commandLine), scriptCommander(StringConverter::TokenizeQuoted(commandLine)), 
	  light({ 0.0f, 10.0f, 0.0f })
{
	Engine::FolderView::GetInstance();
	Engine::MenuBar::GetInstance();
	Engine::Inspector::GetInstance();

	cameras.AddCamera(std::make_unique<Camera>("A", DirectX::XMFLOAT3{ -22.0f, 4.0f, 0.0f }, 0.0f, Math::PI / 2.0f));
	cameras.AddCamera(std::make_unique<Camera>("B", DirectX::XMFLOAT3{ -13.5f,28.8f,-6.4f }, Math::PI / 180.0f * 13.0f, Math::PI / 180.0f * 61.0f));
	cameras.AddCamera(light.GetLightViewCamera());

	currentScene = SponzaScene::Create("Sponza");

	currentScene->Initialize();
	currentScene->BeforeFrame();
	currentScene->Start();
	currentScene->LateStart();

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-2.0f, 13.0f, -10.0f));
	//texturePlane.SetPosition({ -2.0f, 13.0f, -10.0f });
	//texturePlane.SetRotation(0.0f, 3.14f, 0.0f);

	light.LinkTechniques(App::GetRenderGraph());
	cameras.LinkTechniques(App::GetRenderGraph());

	App::GetRenderGraph().RenderShadowCamera(*light.GetLightViewCamera());
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

    Camera& activeCamera = cameras.GetActiveCamera();

	// 활성 카메라의 뷰 행렬과 투영 행렬 구하기
	DirectX::XMMATRIX viewMatrix = activeCamera.GetMatrix();
	DirectX::XMMATRIX projMatrix = activeCamera.GetProjection();
	RenderGraphNameSpace::RenderJob::GetViewFrustum().UpdateFromMatrices(viewMatrix, projMatrix);

	light.Update(cameras->GetMatrix());
	App::GetRenderGraph().RenderMainCamera(cameras.GetActiveCamera());

	currentScene->Update();

	light.Submit(RenderingChannel::main);
	cameras.Submit(RenderingChannel::main);

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
	cameras.CreateWindow();
	light.CreatePositionChangeWindow();
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

		if (wnd.keyBoard.IsPressed('R'))
			LoadScene(SponzaScene::Create("Sponza"));

		if (wnd.keyBoard.IsPressed('T'))
			LoadScene(EmptyScene::Create("EmptyScene"));
	}

	while (const auto mouseDelta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.GetCursorEnabled())
			cameras->Rotate((float)mouseDelta->x, (float)mouseDelta->y);
	}
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
	if (ImGui::Begin("Simulation Speed"))
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