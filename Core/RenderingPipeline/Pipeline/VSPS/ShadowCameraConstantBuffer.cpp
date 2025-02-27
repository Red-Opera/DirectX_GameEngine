#include "stdafx.h"
#include "ShadowCameraConstantBuffer.h"

#include "Core/Camera/Camera.h"

namespace Graphic
{
	ShadowCameraConstantBuffer::ShadowCameraConstantBuffer(DxGraphic& graphic, UINT slot)
		: vertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(graphic, slot) }
	{

	}

	void ShadowCameraConstantBuffer::Update(DxGraphic& graphic)
	{
		const auto position = camera->GetPosition();
		const Transform transform{ DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(-position.x, -position.y, -position.z)) };

		vertexConstantBuffer->Update(graphic, transform);
	}

	void ShadowCameraConstantBuffer::SetCamera(const Camera* camera) noexcept
	{
		this->camera = camera;
	}

	void ShadowCameraConstantBuffer::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		vertexConstantBuffer->SetRenderPipeline(graphic);
	}
}