#include "stdafx.h"
#include "Camera.h"

#include "Core/Window.h"
#include "Core/DxGraphic.h"
#include "Core/Object/Object.h"

Camera::Camera(std::shared_ptr<class Object> object, bool isTethered) noexcept
	: Component(object), projection(1.0f, 9.0f / 16.0f, 0.5f, 400.0f), 
	  indicator(), isTethered(isTethered)
{
	if (isTethered)
	{
		auto position = object->transform->GetPosition();

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}

	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	auto pos = this->object->transform->GetPosition();
    DirectX::XMFLOAT3 position = { pos.x, pos.y, pos.z };
	auto rotation = this->object->transform->GetRotation();

	// 카메라의 앞 방향 Vector와 위치 Vector를 구함
	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(Vector::forwardV, DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, 0.0f));
	const DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);

	// 카메라가 바라보는 목표 위치
	const DirectX::XMVECTOR cameraTarget = positionVector + lookVector;

	// EyePosition과 FocusPosition을 다르게 설정합니다.
	return DirectX::XMMatrixLookAtLH(positionVector, cameraTarget, Vector::upV);
}

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return projection.GetMatrix();
}

void Camera::SpawnControlWidgets() noexcept
{
	auto& position = this->object->transform->GetPosition();
	auto& rotation = this->object->transform->GetRotation();

	// 프로젝션 설정은 유지
	projection.RenderWidgets();

	// 카메라 시각화 옵션도 유지
	ImGui::Checkbox("Enable Camera Indicator", &isEnableIndicator);
	ImGui::Checkbox("Enable Frustum Indicator", &isEnableFrustumIndicator);

	if (ImGui::Button("Reset"))
		Reset();

	// Transform이 외부(Inspector)에서 변경되었을 때 indicator와 projection 업데이트
	indicator.SetPosition(position);
	projection.SetPosition(position);

	const Rotation angle = { rotation.x, rotation.y, 0.0f };
	indicator.SetRotation(angle);
	projection.SetRotation(angle);
}

void Camera::Reset() noexcept
{
	if (!isTethered)
	{
		auto& position = this->object->transform->GetPosition();
		position = { 0.0f, 0.0f, 0.0f };

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}

	auto& rotation = this->object->transform->GetRotation();
	rotation = { 0.0f, 0.0f, 0.0f };

	indicator.SetRotation(rotation);
	projection.SetRotation(rotation);

	projection.Reset();
}

void Camera::Rotate(float dx, float dy) noexcept
{
	auto& rotation = this->object->transform->GetRotation();

	rotation.x = std::clamp(rotation.x + dy * rotationSpeed, 0.995f * -Math::PI / 2.0f, 0.995f * Math::PI / 2.0f);
	rotation.y = Math::NormalizeRadian(rotation.y + dx * rotationSpeed);

	indicator.SetRotation(rotation);
	projection.SetRotation(rotation);
}

void Camera::Translate(Position translation) noexcept
{
	auto& position = this->object->transform->GetPosition();
	auto& rotation = this->object->transform->GetRotation();

	if (!isTethered)
	{
		DirectX::XMFLOAT3 translationFloat3 = { translation.x, translation.y, translation.z };

		DirectX::XMStoreFloat3(&translationFloat3, DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translationFloat3),
			DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, 0.0f) *
			DirectX::XMMatrixScaling(moveSpeed, moveSpeed, moveSpeed)
		));

		position = { position.x + translationFloat3.x, position.y + translationFloat3.y, position.z + translationFloat3.z };

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}
}

void Camera::SetPosition(const Position& targetPosition) noexcept
{
	auto& position = this->object->transform->GetPosition();

	position = targetPosition;

	indicator.SetPosition(targetPosition);
	projection.SetPosition(targetPosition);
}

Position& Camera::GetPosition() const noexcept
{
	return this->object->transform->GetPosition();
}

const std::string Camera::GetName() const noexcept
{
	return object->GetName();
}

void Camera::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	indicator.LinkTechniques(renderGraph);
	projection.LinkTechniques(renderGraph);
}

void Camera::Submit(size_t channel) const
{
	if (isEnableIndicator)
		indicator.Submit(channel);

	if (isEnableFrustumIndicator)
		projection.Submit(channel);
}

void Camera::RenderToGraphic() const
{
	Window::GetDxGraphic().SetCamera(GetMatrix());
	Window::GetDxGraphic().SetProjection(projection.GetMatrix());
}

void Camera::Initialize()
{

}

void Camera::Update()
{
	Component::Update();
}

void Camera::LateUpdate()
{
	Component::LateUpdate();
}
