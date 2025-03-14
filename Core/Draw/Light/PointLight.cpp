#include "stdafx.h"
#include "PointLight.h"

#include "Core/Camera/Camera.h"

#include "Utility/MathInfo.h"

PointLight::PointLight(DirectX::XMFLOAT3 position, float radius) : mesh(radius), cBuffer()
{
	initLightInfo =
	{
		position,
		{ 0.25f, 0.25f, 0.25f },
		{ 1.0f, 1.0f, 1.0f },
		3.0f, 1.0f, 0.025f, 0.030f
	};

	Reset();

	viewCamera = std::make_shared<Camera>("Light", lightInfo.position, 0.0f, Math::PI / 2.0f, true);
}

void PointLight::CreatePositionChangeWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		bool isNotMatch = false;
		const auto IsNotMatch = [&isNotMatch](bool notMatch) { isNotMatch = isNotMatch || notMatch; };

		ImGui::Text("Position");
		IsNotMatch(ImGui::SliderFloat("X", &lightInfo.position.x, -60.0f, 60.0f, "%.1f"));
		IsNotMatch(ImGui::SliderFloat("Y", &lightInfo.position.y, -60.0f, 60.0f, "%.1f"));
		IsNotMatch(ImGui::SliderFloat("Z", &lightInfo.position.z, -60.0f, 60.0f, "%.1f"));

		if (isNotMatch)
			viewCamera->SetPosition(lightInfo.position);

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &lightInfo.diffuseIntensity, 0.01f, 200.0f, "%.2f");
		ImGui::ColorEdit3("Diffuse Color", &lightInfo.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &lightInfo.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &lightInfo.attConst, 0.05f, 10.0f, "%.2f");
		ImGui::SliderFloat("Linear", &lightInfo.attLin, 0.0001f, 4.0f, "%.4f");
		ImGui::SliderFloat("Quadratic", &lightInfo.attQuad, 0.0000001f, 10.0f, "%.7f");

		if (ImGui::Button("Reset"))
			Reset();
	}

	ImGui::End();
}

void PointLight::Reset() noexcept
{
	lightInfo = initLightInfo;
}

void PointLight::Submit(size_t channel) const NOEXCEPTRELEASE
{
	mesh.SetPos(lightInfo.position);
	mesh.Submit(channel);
}

void PointLight::Update(DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = lightInfo;
	const auto position = DirectX::XMLoadFloat3(&lightInfo.position);
	DirectX::XMStoreFloat3(&dataCopy.position, DirectX::XMVector3Transform(position, view));

	cBuffer.Update(dataCopy);
	cBuffer.SetRenderPipeline();
}

void PointLight::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	mesh.LinkTechniques(renderGraph);
}

std::shared_ptr<Camera> PointLight::GetLightViewCamera() const noexcept
{
	return viewCamera;
}