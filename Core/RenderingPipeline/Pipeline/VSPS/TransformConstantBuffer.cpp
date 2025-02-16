#include "stdafx.h"
#include "TransformConstantBuffer.h"

namespace Graphic
{
	TransformConstantBuffer::TransformConstantBuffer(DxGraphic& graphic, UINT slot) 
		: parent(parent)
	{
		if (!vertexConstantBufferMatrix)
			vertexConstantBufferMatrix = std::make_unique<VertexConstantBuffer<Transform>>(graphic, slot);
	}

	void TransformConstantBuffer::InitializeParentReference(const Drawable& parent) noexcept
	{
		this->parent = &parent;
	}

	void TransformConstantBuffer::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);

		GRAPHIC_THROW_INFO_ONLY(UpdateSetRenderPipeline(graphic, GetTransform(graphic)));
	}

	void TransformConstantBuffer::UpdateSetRenderPipeline(DxGraphic& graphic, const Transform& transform) NOEXCEPTRELEASE
	{
		assert(parent != nullptr);

		vertexConstantBufferMatrix->Update(graphic, transform);
		vertexConstantBufferMatrix->SetRenderPipeline(graphic);
	}

	TransformConstantBuffer::Transform TransformConstantBuffer::GetTransform(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		assert(parent != nullptr);

		// 부모의 Transformd에서 카메라 위치를 곱하여 View 위치를 구함
		const auto viewTransform = parent->GetTransformMatrix() * graphic.GetCamera();

		// 상수 버퍼로 만들 Transform을 만듬
		return 
		{
			DirectX::XMMatrixTranspose(viewTransform),

			// World * View * Projection
			DirectX::XMMatrixTranspose(viewTransform * graphic.GetProjection())
		};
	}

	std::unique_ptr<RenderInstance> TransformConstantBuffer::Instance() const noexcept
	{
		return std::make_unique<TransformConstantBuffer>(*this);
	}

	std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transform>> TransformConstantBuffer::vertexConstantBufferMatrix;
}