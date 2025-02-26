#include "stdafx.h"
#include "SkyboxTransformConstantBuffer.h"


namespace Graphic
{
	SkyboxTransformConstantBuffer::SkyboxTransformConstantBuffer(DxGraphic& graphic, UINT slot)
		: vertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(graphic, slot) }
	{

	}

	void SkyboxTransformConstantBuffer::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGER(graphic);

		GRAPHIC_THROW_INFO_ONLY(UpdateRender(graphic, GetTransform(graphic)));
	}

	void SkyboxTransformConstantBuffer::UpdateRender(DxGraphic& graphic, const Transform& transform) NOEXCEPTRELEASE
	{
		vertexConstantBuffer->Update(graphic, transform);
		vertexConstantBuffer->SetRenderPipeline(graphic);
	}

	SkyboxTransformConstantBuffer::Transform SkyboxTransformConstantBuffer::GetTransform(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		return { DirectX::XMMatrixTranspose(graphic.GetCamera() * graphic.GetProjection()) };
	}
}