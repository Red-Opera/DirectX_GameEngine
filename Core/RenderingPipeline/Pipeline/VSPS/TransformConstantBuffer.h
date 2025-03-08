#pragma once

#include "ConstantBuffer.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class TransformConstantBuffer : public RenderInstance
	{
	public:
		TransformConstantBuffer(UINT slot = 0u);

		// Render을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;
		void InitializeParentReference(const Drawable& parent) noexcept override;

		// RenderInstance을(를) 통해 상속됨
		std::unique_ptr<RenderInstance> Instance() const noexcept override;

	protected:
		struct Transform
		{
			DirectX::XMMATRIX worldViewProjection;
			DirectX::XMMATRIX transform;
			DirectX::XMMATRIX model;
		};

		void UpdateSetRenderPipeline(const Transform& transform) NOEXCEPTRELEASE;
		Transform GetTransform() NOEXCEPTRELEASE;

		static std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBufferMatrix;
		const Drawable* parent = nullptr;
	};
}