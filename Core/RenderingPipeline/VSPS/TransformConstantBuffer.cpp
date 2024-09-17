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
		// 부모의 Transformd에서 카메라 위치를 곱하여 View 위치를 구함
		const auto viewTransform = parent.GetTransformMatrix() * graphic.GetCamera();

		// 상수 버퍼로 만들 Transform을 만듬
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