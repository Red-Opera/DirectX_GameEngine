#include "stdafx.h"
#include "Camera.h"

#include "Core/DxGraphic.h"

Camera::Camera(DxGraphic& graphic, std::string name, DirectX::XMFLOAT3 initPosition, float initPitch, float initYaw) noexcept
	: name(name), initPosition(initPosition), initPitch(initPitch), initYaw(initYaw), projection(graphic, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f), indicator(graphic)
{
	Reset(graphic);
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// ī�޶��� �� ���� Vector�� ��ġ Vector�� ����
	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(Vector::forwardV, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
	const DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);

	// ī�޶� �ٶ󺸴� ��ǥ ��ġ
	const DirectX::XMVECTOR cameraTarget = positionVector + lookVector;

	// EyePosition�� FocusPosition�� �ٸ��� �����մϴ�.
	return DirectX::XMMatrixLookAtLH(positionVector, cameraTarget, Vector::upV);
}

void Camera::SpawnControlWidgets(DxGraphic& graphic) noexcept
{
	bool isRotationIsNotMatch = false;
	bool isPositionIsNotMatch = false;
	const auto IsNotMatch = [](bool notMatch, bool& carry) { carry = carry || notMatch; };

	ImGui::Text("Position");
	IsNotMatch(ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f"), isPositionIsNotMatch);
	IsNotMatch(ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f"), isPositionIsNotMatch);
	IsNotMatch(ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f"), isPositionIsNotMatch);

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
	position = initPosition;

	pitch = initPitch;
	yaw = initYaw;
	
	indicator.SetPosition(position);
	projection.SetPosition(position);

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
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		DirectX::XMMatrixScaling(moveSpeed, moveSpeed, moveSpeed)
	));

	position = { position.x + translation.x, position.y + translation.y, position.z + translation.z };

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

void Camera::Submit() const
{
	if (isEnableIndicator)
		indicator.Submit();

	if (isEnableFrustumIndicator)
		projection.Submit();
}

void Camera::RenderToGraphic(DxGraphic& graphic) const
{
	graphic.SetCamera(GetMatrix());
	graphic.SetProjection(projection.GetMatrix());
}
