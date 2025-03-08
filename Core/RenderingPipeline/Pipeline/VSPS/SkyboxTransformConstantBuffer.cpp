#include "stdafx.h"
#include "SkyboxTransformConstantBuffer.h"


namespace Graphic
{
	SkyboxTransformConstantBuffer::SkyboxTransformConstantBuffer(UINT slot)
		: vertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(slot) }
	{

	}

	void SkyboxTransformConstantBuffer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(UpdateRender(GetTransform()));
	}

	void SkyboxTransformConstantBuffer::UpdateRender(const Transform& transform) NOEXCEPTRELEASE
	{
		vertexConstantBuffer->Update(transform);
		vertexConstantBuffer->SetRenderPipeline();
	}

	SkyboxTransformConstantBuffer::Transform SkyboxTransformConstantBuffer::GetTransform() NOEXCEPTRELEASE
	{
		return { DirectX::XMMatrixTranspose(Window::GetDxGraphic().GetCamera() * Window::GetDxGraphic().GetProjection())};
	}
}