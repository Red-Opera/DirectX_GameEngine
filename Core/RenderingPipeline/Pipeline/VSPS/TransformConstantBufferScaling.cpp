#include "stdafx.h"
#include "TransformConstantBufferScaling.h"

#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

namespace Graphic
{
	TransformConstantBufferScaling::TransformConstantBufferScaling(float scale)
		: TransformConstantBuffer(), buffer(CreateVertexLayout())
	{
		buffer["scale"] = scale;
	}

	void TransformConstantBufferScaling::Accept(TechniqueBase& techniqueBase)
	{
		techniqueBase.VisitBuffer(buffer);
	}

	void TransformConstantBufferScaling::SetRenderPipeline() noexcept
	{
		const float scale = buffer["scale"];
		const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
		auto transform = GetTransform();

		transform.transform = transform.transform * scaleMatrix;
		transform.worldViewProjection = transform.worldViewProjection * scaleMatrix;

		UpdateSetRenderPipeline(transform);
	}

	std::unique_ptr<RenderInstance> TransformConstantBufferScaling::Instance() const noexcept
	{
		return std::make_unique<TransformConstantBufferScaling>(*this);
	}

	DynamicConstantBuffer::EditLayout TransformConstantBufferScaling::CreateVertexLayout()
	{
		DynamicConstantBuffer::EditLayout layout;
		layout.add<DynamicConstantBuffer::float1>("scale");

		return layout;
	}
}
