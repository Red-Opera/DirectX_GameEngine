#include "stdafx.h"
#include "App.h"

#include "Core/Draw/DrawBox.h"
#include "Core/Draw/Melon.h"
#include "Core/Draw/Pyramid.h"
#include "Core/Draw/Sheet.h"
#include "Core/Draw/TextureCube.h"
#include "Core/Draw/Cylinder.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Draw/Base/Image/GDIPlus.h"

#include "Utility/Imgui/imgui.h"

#include <iomanip>
using namespace std;

GDIPlus gdiPlus;

App::App() : wnd(800, 600, "Make Box Game"), light(wnd.GetDxGraphic())
{
	class Factory
	{
	public:
		Factory(DxGraphic& graphic) : graphic(graphic) {}

		unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 matrix = { cdist(rng), cdist(rng), cdist(rng) };

			switch (sdist(rng))
			{
			case 0:
				return make_unique<DrawBox>(graphic, rng, adist, ddist, odist, rdist, bdist, matrix);
			case 1:
				return make_unique<Cylinder>(graphic, rng, adist, ddist, odist, rdist, bdist, tdist);
			case 2:
				return make_unique<Pyramid>(graphic, rng, adist, ddist, odist, rdist, tdist);
			case 3:
				return make_unique<TextureCube>(graphic, rng, adist, ddist, odist, rdist);
			//case 3:
			//	return make_unique<Sheet>(graphic, rng, adist, ddist, odist, rdist);
			
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		DxGraphic& graphic;
		mt19937 rng{ random_device{}() };
		uniform_int_distribution<int> sdist{ 0,3 };
		uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
		uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		uniform_int_distribution<int> latdist{ 5,20 };
		uniform_int_distribution<int> longdist{ 10,40 };
		uniform_int_distribution<int> typedist{ 0, 4 };
		uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		uniform_int_distribution<int> tdist{ 3,30 };
	};

	Factory f(wnd.GetDxGraphic());
	drawables.reserve(drawableCount);
	std::generate_n(std::back_inserter(drawables), drawableCount, f);

	// init box pointers for editing instance parameters
	for (auto& drawable : drawables)
	{
		if (auto out = dynamic_cast<DrawBox*>(drawable.get()))
			boxes.push_back(out);
	}

	wnd.GetDxGraphic().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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

	ostringstream out;
	out << "Play Time : " << setprecision(1) << fixed << t << "s";
	wnd.GetDxGraphic().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.GetDxGraphic().SetCamera(camera.GetMatrix());
	light.Bind(wnd.GetDxGraphic(), camera.GetMatrix());

	// 모든 기하구조를 업데이트한 후 그림
	for (auto& box : drawables)
	{
		box->Update(wnd.keyBoard.IsPressed(VK_SPACE) ? 0.0f : deltaTime * objectSpeed);
		box->Draw(wnd.GetDxGraphic());
	}
	light.Draw(wnd.GetDxGraphic());

	CreateSimulationWindow();
	camera.SpawnControlWindow();
	light.CreatePositionChangeWindow();

	// imgui window to open box windows
	CreateBoxWindowManagerWindow();

	// 박스 상태 창 생성
	CreateBoxWindows();
	
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

void App::CreateBoxWindows() noexcept
{
	for (auto i = boxControlIds.begin(); i != boxControlIds.end(); )
	{
		if (!boxes[*i]->CreateControlWindow(*i, wnd.GetDxGraphic()))
			i = boxControlIds.erase(i);

		else
			i++;
	}
}