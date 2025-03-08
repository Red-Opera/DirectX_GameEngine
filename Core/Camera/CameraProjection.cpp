#include "stdafx.h"
#include "CameraProjection.h"

#include "Core/DxGraphic.h"

CameraProjection::CameraProjection(float width, float height, float nearZ, float farZ)
	: width(width), height(height), nearZ(nearZ), farZ(farZ), frust(width, height, nearZ, farZ),
	  initWidth(width), initHeight(height), initNearZ(nearZ), initFarZ(farZ)
{

}

void CameraProjection::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	frust.SetPosition(position);
}

void CameraProjection::SetRotation(DirectX::XMFLOAT3 rotation) noexcept
{
	frust.SetRotation(rotation);
}

DirectX::XMMATRIX CameraProjection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH(width, height, nearZ, farZ);
}

void CameraProjection::Submit(size_t channel) const
{
	frust.Submit(channel);
}

void CameraProjection::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	frust.LinkTechniques(renderGraph);
}

void CameraProjection::RenderWidgets()
{
	bool isNotMatch = false;
	const auto MatchCheck = [&isNotMatch](bool notMatch) { isNotMatch = isNotMatch || notMatch; };

	ImGui::Text("Projection");
	MatchCheck(ImGui::SliderFloat("Width", &width, 0.01f, 4.0f, "%.2f"));
	MatchCheck(ImGui::SliderFloat("Height", &height, 0.01f, 4.0f, "%.2f"));
	MatchCheck(ImGui::SliderFloat("Near Z", &nearZ, 0.01f, farZ - 0.01f, "%.2f"));
	MatchCheck(ImGui::SliderFloat("Far Z", &farZ, nearZ + 0.01f, 400.0f, "%.2f"));

	if (isNotMatch)
		frust.SetVertices(width, height, nearZ, farZ);
}

void CameraProjection::Reset()
{
	width = initWidth;
	height = initHeight;
	nearZ = initNearZ;
	farZ = initFarZ;

	frust.SetVertices(width, height, nearZ, farZ);
}
