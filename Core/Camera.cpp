#include "stdafx.h"
#include "Camera.h"

#include "Utility/Imgui/imgui.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// 카메라의 위치를 -r만큼 이동 시키고 phi를 통해 위아래, theta를 통해서 왼오른쪽으로 회전하여 위치를 바꿈
	const auto position = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f));

	// View 행렬을 계산하여 반환함 (해당 위치에서 원점을 바라보고 위쪽은 y이며, pitch, -yaw, roll에 따라 카메라를 회전함)
	return DirectX::XMMatrixLookAtLH(position, DirectX::XMVectorZero(), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) *
		   DirectX::XMMatrixRotationRollPitchYaw(pitch, -yaw, roll);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera Transform"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -89.0f, 89.0f);

		ImGui::Text("Rotation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
			Reset();
	}

	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;

	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}