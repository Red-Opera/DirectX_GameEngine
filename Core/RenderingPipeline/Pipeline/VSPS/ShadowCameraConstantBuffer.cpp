#include "stdafx.h"
#include "ShadowCameraConstantBuffer.h"

#include "Core/Camera/Camera.h"

namespace Graphic
{
	ShadowCameraConstantBuffer::ShadowCameraConstantBuffer(UINT slot)
		: vertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(slot) }
	{

	}

	void ShadowCameraConstantBuffer::Update()
	{
		const auto position = camera->GetPosition();
		const Transform transform{ DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(-position.x, -position.y, -position.z)) };

		vertexConstantBuffer->Update(transform);
	}

	void ShadowCameraConstantBuffer::SetCamera(const Camera* camera) noexcept
	{
		this->camera = camera;
	}

	void ShadowCameraConstantBuffer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		vertexConstantBuffer->SetRenderPipeline();
	}
}