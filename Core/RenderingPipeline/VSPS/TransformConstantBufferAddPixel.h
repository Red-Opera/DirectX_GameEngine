#pragma once

#include "TransformConstantBuffer.h"

namespace Graphic
{
	class TransformConstantBufferAddPixel : public TransformConstantBuffer
	{
	public:
		TransformConstantBufferAddPixel(DxGraphic& graphic, const Drawable& parent, UINT vertexSlot = 0u, UINT pixelSlot = 0u);

		// TransformConstantBuffer을(를) 통해 상속됨
		void PipeLineSet(DxGraphic& graphic) noexcept override;

	protected:
		void UpdatePipeLineSet(DxGraphic& graphic, const Transform& transform) noexcept;

	private:
		static std::unique_ptr<PixelConstantBuffer<Transform>> pixelConstantBuffer;
	};
}