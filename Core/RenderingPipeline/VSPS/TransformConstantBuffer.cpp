#include "stdafx.h"
#include "TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(DxGraphic& graphic, const Drawable& parent)
	: parent(parent)
{
	if (!vertexConstantBufferMatrix)
		vertexConstantBufferMatrix = make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphic);
}

void TransformConstantBuffer::PipeLineSet(DxGraphic& graphic) noexcept
{
	vertexConstantBufferMatrix->Update(graphic, DirectX::XMMatrixTranspose(parent.GetTransformXM() * graphic.GetProjection()));

	vertexConstantBufferMatrix->PipeLineSet(graphic);
}

unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformConstantBuffer::vertexConstantBufferMatrix;