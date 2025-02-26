#include "stdafx.h"
#include "Camera.h"

#include "Core/DxGraphic.h"

Camera::Camera(DxGraphic& graphic, std::string name, DirectX::XMFLOAT3 initPosition, float initPitch, float initYaw, bool isTethered) noexcept
	: name(std::move(name)), initPosition(initPosition), initPitch(initPitch), initYaw(initYaw), projection(graphic, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f), 
	  indicator(graphic), isTethered(isTethered)
{
	if (isTethered)
	{
		position = initPosition;

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}

	Reset(graphic);
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// 카메라의 앞 방향 Vector와 위치 Vector를 구함
	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(Vector::forwardV, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
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

void Camera::SpawnControlWidgets(DxGraphic& graphic) noexcept
{
	bool isRotationIsNotMatch = false;
	bool isPositionIsNotMatch = false;
	const auto IsNotMatch = [](bool notMatch, bool& carry) { carry = carry || notMatch; };

	if (!isTethered)
	{
		ImGui::Text("Position");
		IsNotMatch(ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f"), isPositionIsNotMatch);
		IsNotMatch(ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f"), isPositionIsNotMatch);
		IsNotMatch(ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f"), isPositionIsNotMatch);
	}

	ImGui::Text("Rotation");
	IsNotMatch(ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f), isRotationIsNotMatch);
	IsNotMatch(ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f), isRotationIsNotMatch);

	projection.RenderWidgets(graphic);

	ImGui::Checkbox("Enable Camera Indicator", &isEnableIndicator);
	ImGui::Checkbox("Enable Frustum Indicator", &isEnableFrustumIndicator);

	if (ImGui::Button("Reset"))
		Reset(graphic);

	if (isRotationIsNotMatch)
	{
		const DirectX::XMFLOAT3 angle = { pitch, yaw, 0.0f };
		indicator.SetRotation(angle);
		projection.SetRotation(angle);
	}

	if (isPositionIsNotMatch)
	{
		indicator.SetPosition(position);
		projection.SetPosition(position);
	}
}

void Camera::Reset(DxGraphic& graphic) noexcept
{
	if (!isTethered)
	{
		position = initPosition;

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}

	pitch = initPitch;
	yaw = initYaw;

	const DirectX::XMFLOAT3 angle = { pitch, yaw, 0.0f };
	indicator.SetRotation(angle);
	projection.SetRotation(angle);

	projection.Reset(graphic);
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = Math::NormalizeRadian(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -Math::PI / 2.0f, 0.995f * Math::PI / 2.0f);

	const DirectX::XMFLOAT3 angle = { pitch, yaw, 0.0f };
	indicator.SetRotation(angle);
	projection.SetRotation(angle);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	if (!isTethered)
	{
		DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
			DirectX::XMMatrixScaling(moveSpeed, moveSpeed, moveSpeed)
		));

		position = { position.x + translation.x, position.y + translation.y, position.z + translation.z };

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}
}

void Camera::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	this->position = position;

	indicator.SetPosition(position);
	projection.SetPosition(position);
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return position;
}

const std::string& Camera::GetName() const noexcept
{
	return name;
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

void Camera::RenderToGraphic(DxGraphic& graphic) const
{
	graphic.SetCamera(GetMatrix());
	graphic.SetProjection(projection.GetMatrix());
}
