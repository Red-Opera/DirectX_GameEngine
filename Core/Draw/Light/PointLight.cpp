#include "stdafx.h"
#include "PointLight.h"

#include "Core/App.h"
#include "Core/Camera/Camera.h"
#include "Core/Object/Object.h"
#include "Core/Component/Component.h"
#include "Core/Draw/Object/ColorSphereObject.h"
#include "Core/Component/TransformComponent.h"
#include "Core/RenderingPipeline/RenderingChannel.h"

#include "Utility/MathInfo.h"

PointLight::PointLight(std::shared_ptr<Object> object, Position position, float radius)
	: Component(object), mesh(this->object->AddComponent<ColorSphereObject>()), cBuffer()
{
	this->object->transform->SetScale(radius, radius, radius);
	this->object->GetComponent<ColorSphereObject>()->SetLit(false);

	initLightInfo =
	{
		position,
		{ 0.25f, 0.25f, 0.25f },
		{ 1.0f, 1.0f, 1.0f },
		3.0f, 1.0f, 0.025f, 0.030f
	};

	Reset();

	viewCamera = Object::Create("LightCamera");
	viewCamera->AddComponent<Camera>("LightCamera", true);
	viewCamera->GetComponent<TransformComponent>()->SetPosition(lightInfo.position);
	viewCamera->GetComponent<TransformComponent>()->SetRotation(0.0f, Math::PI / 2.0f, 0.0f);
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
			viewCamera->GetComponent<Camera>()->SetPosition(lightInfo.position);

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

	// Update transform if available
	if (transform)
		transform->SetPosition(lightInfo.position);
}

void PointLight::Submit(size_t channel) const NOEXCEPTRELEASE
{
	transform->SetPosition(lightInfo.position);
}

std::shared_ptr<Object> PointLight::GetLightViewCamera() const noexcept
{
	return viewCamera;
}

void PointLight::Initialize()
{
	mesh->LinkTechniques(App::GetRenderGraph());
}

void PointLight::Update()
{
	Component::Update();

	auto& activeCamera = CameraContainer::GetActiveCamera();
	const DirectX::FXMMATRIX view = activeCamera.GetMatrix();

	auto dataCopy = lightInfo;
    const auto position = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&lightInfo.position));

	dataCopy.position = DirectX::XMVector3Transform(position, view);

	cBuffer.Update(dataCopy);
	cBuffer.SetRenderPipeline();

	Submit(RenderingChannel::main);
}

void PointLight::LateUpdate()
{
	Component::LateUpdate();
}
