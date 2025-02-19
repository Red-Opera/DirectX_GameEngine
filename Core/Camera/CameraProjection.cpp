#include "stdafx.h"
#include "CameraProjection.h"

#include "Core/DxGraphic.h"

CameraProjection::CameraProjection(DxGraphic& graphic, float width, float height, float nearZ, float farZ)
	: width(width), height(height), nearZ(nearZ), farZ(farZ), frust(graphic, width, height, nearZ, farZ),
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

void CameraProjection::Submit() const
{
	frust.Submit();
}

void CameraProjection::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	frust.LinkTechniques(renderGraph);
}

void CameraProjection::RenderWidgets(DxGraphic& graphic)
{
	bool isNotMatch = false;
	const auto MatchCheck = [&isNotMatch](bool notMatch) { isNotMatch = isNotMatch || notMatch; };


	ImGui::Text("Projection");
	MatchCheck(ImGui::SliderFloat("Width", &width, 0.01f, 4.0f, "%.2f", 1.5f));
	MatchCheck(ImGui::SliderFloat("Height", &height, 0.01f, 4.0f, "%.2f", 1.5f));
	MatchCheck(ImGui::SliderFloat("Near Z", &nearZ, 0.01f, farZ - 0.01f, "%.2f", 4.0f));
	MatchCheck(ImGui::SliderFloat("Far Z", &farZ, nearZ + 0.01f, 400.0f, "%.2f", 4.0f));

	if (isNotMatch)
		frust.SetVertices(graphic, width, height, nearZ, farZ);
}

void CameraProjection::Reset(DxGraphic& graphic)
{
	width = initWidth;
	height = initHeight;
	nearZ = initNearZ;
	farZ = initFarZ;

	frust.SetVertices(graphic, width, height, nearZ, farZ);
}
