#include "stdafx.h"
#include "TransformConstantBufferAddPixel.h"

namespace Graphic
{
	TransformConstantBufferAddPixel::TransformConstantBufferAddPixel(DxGraphic& graphic, const Drawable& parent, UINT vertexSlot, UINT pixelSlot) : TransformConstantBuffer(graphic, parent, vertexSlot)
	{
		if (!pixelConstantBuffer)
			pixelConstantBuffer = std::make_unique<PixelConstantBuffer<Transform>>(graphic, pixelSlot);
	}

	void TransformConstantBufferAddPixel::PipeLineSet(DxGraphic& graphic) noexcept
	{
		const auto transform = GetTransform(graphic);

		TransformConstantBuffer::UpdatePipeLineSet(graphic, transform);
		UpdatePipeLineSet(graphic, transform);
	}

	void TransformConstantBufferAddPixel::UpdatePipeLineSet(DxGraphic& graphic, const Transform& transform) noexcept
	{
		pixelConstantBuffer->Update(graphic, transform);
		pixelConstantBuffer->PipeLineSet(graphic);
	}

	std::unique_ptr<PixelConstantBuffer<TransformConstantBuffer::Transform>> TransformConstantBufferAddPixel::pixelConstantBuffer;
}