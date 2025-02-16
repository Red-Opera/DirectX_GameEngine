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

		// �θ��� Transformd���� ī�޶� ��ġ�� ���Ͽ� View ��ġ�� ����
		const auto viewTransform = parent->GetTransformMatrix() * graphic.GetCamera();

		// ��� ���۷� ���� Transform�� ����
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