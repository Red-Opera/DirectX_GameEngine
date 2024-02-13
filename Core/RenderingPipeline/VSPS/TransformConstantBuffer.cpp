#include "stdafx.h"
#include "TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(DxGraphic& graphic, const Drawable& parent)
	: parent(parent)
{
	if (!vertexConstantBufferMatrix)
		vertexConstantBufferMatrix = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphic);
}

void TransformConstantBuffer::PipeLineSet(DxGraphic& graphic) noexcept
{
	vertexConstantBufferMatrix->Update(graphic, 
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() *		// World
			graphic.GetCamera() *			// View
			graphic.GetProjection()));		// Projection

	vertexConstantBufferMatrix->PipeLineSet(graphic);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformConstantBuffer::vertexConstantBufferMatrix;