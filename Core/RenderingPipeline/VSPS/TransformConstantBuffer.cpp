#include "stdafx.h"
#include "TransformConstantBuffer.h"

namespace Graphic
{
	TransformConstantBuffer::TransformConstantBuffer(DxGraphic& graphic, const Drawable& parent, UINT slot) : parent(parent)
	{
		if (!vertexConstantBufferMatrix)
			vertexConstantBufferMatrix = std::make_unique<VertexConstantBuffer<Transform>>(graphic, slot);
	}

	void TransformConstantBuffer::PipeLineSet(DxGraphic& graphic) noexcept
	{
		// �θ��� Transformd���� ī�޶� ��ġ�� ���Ͽ� View ��ġ�� ����
		const auto viewTransform = parent.GetTransformMatrix() * graphic.GetCamera();

		// ��� ���۷� ���� Transform�� ����
		const Transform transform =
		{
			DirectX::XMMatrixTranspose(viewTransform),

			// World * View * Projection
			DirectX::XMMatrixTranspose(viewTransform * graphic.GetProjection())
		};

		vertexConstantBufferMatrix->Update(graphic, transform);
		vertexConstantBufferMatrix->PipeLineSet(graphic);
	}

	std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transform>> TransformConstantBuffer::vertexConstantBufferMatrix;
}