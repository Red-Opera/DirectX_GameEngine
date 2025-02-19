#pragma once

#include "CameraFrustum.h"

#include <DirectXMath.h>

namespace RenderGraphNameSpace { class RenderGraph; }

class DxGraphic;

class CameraProjection
{
public:
	CameraProjection(DxGraphic& graphic, float width, float height, float nearZ, float farZ);

	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rotation) noexcept;
	DirectX::XMMATRIX GetMatrix() const;

	void Submit() const;
	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);

	void RenderWidgets(DxGraphic& graphic);

	void Reset(DxGraphic& graphic);

private:
	float width, height;
	float nearZ, farZ;

	float initWidth, initHeight;
	float initNearZ, initFarZ;

	CameraFrustum frust;
};