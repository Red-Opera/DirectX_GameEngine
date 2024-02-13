#include "stdafx.h"
#include "Camera.h"

#include "Utility/Imgui/imgui.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// ī�޶��� ��ġ�� -r��ŭ �̵� ��Ű�� phi�� ���� ���Ʒ�, theta�� ���ؼ� �޿��������� ȸ���Ͽ� ��ġ�� �ٲ�
	const auto position = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f));

	// View ����� ����Ͽ� ��ȯ�� (�ش� ��ġ���� ������ �ٶ󺸰� ������ y�̸�, pitch, -yaw, roll�� ���� ī�޶� ȸ����)
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