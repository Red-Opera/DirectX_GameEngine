#pragma once

#include "TransformConstantBuffer.h"

namespace Graphic
{
	class TransformConstantBufferAddPixel : public TransformConstantBuffer
	{
	public:
		TransformConstantBufferAddPixel(DxGraphic& graphic, const Drawable& parent, UINT vertexSlot = 0u, UINT pixelSlot = 0u);

		// TransformConstantBuffer��(��) ���� ��ӵ�
		void PipeLineSet(DxGraphic& graphic) noexcept override;

	protected:
		void UpdatePipeLineSet(DxGraphic& graphic, const Transform& transform) noexcept;

	private:
		static std::unique_ptr<PixelConstantBuffer<Transform>> pixelConstantBuffer;
	};
}