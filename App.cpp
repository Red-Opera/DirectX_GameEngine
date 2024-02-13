#include "stdafx.h"
#include "App.h"

#include "Core/Draw/DrawBox.h"
#include "Core/Draw/Melon.h"
#include "Core/Draw/Pyramid.h"
#include "Core/Draw/Sheet.h"
#include "Core/Draw/TextureCube.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Draw/Base/Image/GDIPlus.h"

#include "Utility/Imgui/imgui.h"

#include <iomanip>
using namespace std;

GDIPlus gdiPlus;

App::App() : wnd(800, 600, "Make Box Game")
{
	class Factory
	{
	public:
		Factory(DxGraphic& graphic) : graphic(graphic) {}

		unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return make_unique<Pyramid>(graphic, rng, adist, ddist, odist, rdist);
			case 1:
				return make_unique<DrawBox>(graphic, rng, adist, ddist, odist, rdist, bdist);
			case 2:
				return make_unique<Melon>(graphic, rng, adist, ddist, odist, rdist, longdist, latdist);
			case 3:
				return make_unique<Sheet>(graphic, rng, adist, ddist, odist, rdist);
			case 4:
				return make_unique<TextureCube>(graphic, rng, adist, ddist, odist, rdist);

			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		DxGraphic& graphic;
		mt19937 rng{ random_device{}() };
		uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
		uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		uniform_int_distribution<int> latdist{ 5,20 };
		uniform_int_distribution<int> longdist{ 10,40 };
		uniform_int_distribution<int> typedist{ 0, 4 };
	};

	Factory f(wnd.GetDxGraphic());
	drawables.reserve(drawableCount);
	generate_n(std::back_inserter(drawables), drawableCount, f);

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

	// 모든 기하구조를 업데이트한 후 그림
	for (auto& box : drawables)
	{
		box->Update(wnd.keyBoard.IsPressed(VK_SPACE) ? 0.0f : deltaTime * objectSpeed);
		box->Draw(wnd.GetDxGraphic());
	}

	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed", &objectSpeed, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.keyBoard.IsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}
	ImGui::End();
	camera.SpawnControlWindow();
	
	wnd.SetTitle(out.str());		// 제목 동기화
	wnd.GetDxGraphic().EndFrame();	// 그래픽 마지막에 실행할 내용
}