#include "stdafx.h"
#include "Camera.h"

Camera::Camera() noexcept
{
	Reset();
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

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera Transform"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");

		ImGui::Text("Rotation");
		ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
			Reset();
	}

	ImGui::End();
}

void Camera::Reset() noexcept
{
	position = { -22.0f, 4.0f, 0.0f };

	pitch = 0.0f;
	yaw = 1.57f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = Math::NormalizeRadian(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -Math::PI / 2.0f, 0.995f * Math::PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		DirectX::XMMatrixScaling(moveSpeed, moveSpeed, moveSpeed)
	));

	position = { position.x + translation.x, position.y + translation.y, position.z + translation.z };
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return position;
}