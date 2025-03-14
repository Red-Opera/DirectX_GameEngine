#pragma once

#include "CameraFrustum.h"

#include <DirectXMath.h>

namespace RenderGraphNameSpace { class RenderGraph; }

class DxGraphic;

class CameraProjection
{
public:
	CameraProjection(float width, float height, float nearZ, float farZ);

	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rotation) noexcept;
	DirectX::XMMATRIX GetMatrix() const;

	void Submit(size_t channel) const;
	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);

	void RenderWidgets();

	void Reset();

private:
	float width, height;
	float nearZ, farZ;

	float initWidth, initHeight;
	float initNearZ, initFarZ;

	CameraFrustum frust;
};