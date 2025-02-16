#include "stdafx.h"
#include "PointLight.h"

PointLight::PointLight(DxGraphic& graphic, float radius) : mesh(graphic, radius), cBuffer(graphic)
{
	Reset();
}

void PointLight::CreatePositionChangeWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &lightInfo.position.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &lightInfo.position.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &lightInfo.position.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &lightInfo.diffuseIntensity, 0.01f, 200.0f, "%.2f", 2);
		ImGui::ColorEdit3("Diffuse Color", &lightInfo.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &lightInfo.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &lightInfo.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &lightInfo.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &lightInfo.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		if (ImGui::Button("Reset"))
			Reset();
	}

	ImGui::End();
}

void PointLight::Reset() noexcept
{
	lightInfo = { 
		{ 0.0f, 10.0f, 0.0f },
		{ 0.4f, 0.4f, 0.4f },
		{ 1.0f, 1.0f, 1.0f },
		3.0f, 1.0f, 0.045f, 0.075f };
}

void PointLight::Submit() const NOEXCEPTRELEASE
{
	mesh.SetPos(lightInfo.position);
	mesh.Submit();
}

void PointLight::Update(DxGraphic& graphic, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = lightInfo;
	const auto position = DirectX::XMLoadFloat3(&lightInfo.position);
	DirectX::XMStoreFloat3(&dataCopy.position, DirectX::XMVector3Transform(position, view));

	cBuffer.Update(graphic, dataCopy);
	cBuffer.SetRenderPipeline(graphic);
}

void PointLight::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	mesh.LinkTechniques(renderGraph);
}
