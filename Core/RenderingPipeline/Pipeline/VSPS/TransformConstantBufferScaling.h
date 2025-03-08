#pragma once

#include "TransformConstantBuffer.h"
#include "DynamicConstantBuffer.h"

namespace Graphic
{
	class TransformConstantBufferScaling : public TransformConstantBuffer
	{
	public:
		TransformConstantBufferScaling(float scale);

		void Accept(TechniqueBase& techniqueBase) override;
		void SetRenderPipeline() noexcept override;

		std::unique_ptr<RenderInstance> Instance() const noexcept;

	private:
		static DynamicConstantBuffer::EditLayout CreateVertexLayout();
		
	private:
		DynamicConstantBuffer::Buffer buffer;
	};
}