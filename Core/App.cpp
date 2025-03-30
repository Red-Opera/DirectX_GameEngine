#include "stdafx.h"
#include "App.h"
#include "TestModelBase.h"

#include "Camera/Camera.h"
#include "Draw/Object/ColorConeObject.h"
#include "Draw/Object/ColorCubeObject.h"
#include "Draw/Object/ColorCylinderObject.h"
#include "Draw/Object/ColorPlaneObject.h"
#include "Draw/Object/ColorSphereObject.h"
#include "Draw/Object/TextureCubeObject.h"
#include "EngineUI/FolderViewInspector.h"
#include "EngineUI/MenuBar.h"
#include "RenderingPipeline/RenderingChannel.h"
#include "RenderingPipeline/Test.h"

#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"

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

	objects.push_back(Object::Create("Texture Cube 1"));
	objects[0]->AddComponent<TextureCubeObject>("Images/brickwall.jpg");

	for (auto& object : objects)
		object->Initialize();

	for (auto& object : objects)
		object->BeforeFrame();

	for (auto& object : objects)
		object->Start();

	for (auto& object : objects)
		object->LateStart();

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-2.0f, 13.0f, -10.0f));
	//texturePlane.SetPosition({ -2.0f, 13.0f, -10.0f });
	//texturePlane.SetRotation(0.0f, 3.14f, 0.0f);
	//gobber.SetRootTransform(DirectX::XMMatrixTranslation(9.2f, 7.0f, 0.0f));
	//bluePlane.SetPosition(camera.GetPosition());
	//redPlane.SetPosition(camera.GetPosition());

	nano.SetRootTransform(
		DirectX::XMMatrixRotationY(Math::PI / 2.f) *
		DirectX::XMMatrixTranslation(27.f, -0.56f, 1.7f)
	);
	gobber.SetRootTransform(
		DirectX::XMMatrixRotationY(-Math::PI / 2.f) *
		DirectX::XMMatrixTranslation(-30.f, 10.f, 0.f)
	);

	light.LinkTechniques(App::GetRenderGraph());
	sponza.LinkTechniques(App::GetRenderGraph());
	gobber.LinkTechniques(App::GetRenderGraph());
	nano.LinkTechniques(App::GetRenderGraph());
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

	for (auto& object : objects)
		object->Update();

	light.Submit(RenderingChannel::main);
	sponza.Submit(RenderingChannel::main);
	nano.Submit(RenderingChannel::main);
	gobber.Submit(RenderingChannel::main);
	cameras.Submit(RenderingChannel::main);

	sponza.Submit(RenderingChannel::shadow);
	gobber.Submit(RenderingChannel::shadow);
	nano.Submit(RenderingChannel::shadow);

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

	for (auto& object : objects)
		object->LateUpdate();

	App::GetRenderGraph().RenderWindows();

	Engine::FolderViewInspector::instance->RenderFolderView();
	Engine::FolderViewInspector::instance->RenderInspector();
	Engine::MenuBar::menuBar->RenderMenuBar();

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

		if (wnd.keyBoard.IsPressed('P'))
			objects[0]->transform->SetPosition(objects[0]->transform->GetRight() * cameraDelta + objects[0]->transform->GetPosition());

		if (wnd.keyBoard.IsPressed('L'))
			objects[0]->transform->SetPosition(objects[0]->transform->GetRight() * -cameraDelta + objects[0]->transform->GetPosition());

		if (wnd.keyBoard.IsPressed('O'))
			objects[0]->transform->SetPosition(objects[0]->transform->GetForward() * cameraDelta + objects[0]->transform->GetPosition());

		if (wnd.keyBoard.IsPressed('K'))
			objects[0]->transform->SetPosition(objects[0]->transform->GetForward() * -cameraDelta + objects[0]->transform->GetPosition());

		if (wnd.keyBoard.IsPressed('I'))
			objects[0]->transform->SetPosition(objects[0]->transform->GetUp() * cameraDelta + objects[0]->transform->GetPosition());

		if (wnd.keyBoard.IsPressed('J'))
			objects[0]->transform->SetPosition(objects[0]->transform->GetUp() * -cameraDelta + objects[0]->transform->GetPosition());
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