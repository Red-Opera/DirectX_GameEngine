#pragma once

#include "ConstantBuffer.h"

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class TransformConstantBuffer : public RenderInstance
	{
	public:
		TransformConstantBuffer(DxGraphic& graphic, UINT slot = 0u);

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;
		void InitializeParentReference(const Drawable& parent) noexcept override;

		// RenderInstance��(��) ���� ��ӵ�
		std::unique_ptr<RenderInstance> Instance() const noexcept override;

	protected:
		struct Transform
		{
			DirectX::XMMATRIX worldViewProjection;
			DirectX::XMMATRIX transform;
			DirectX::XMMATRIX model;
		};

		void UpdateSetRenderPipeline(DxGraphic& graphic, const Transform& transform) NOEXCEPTRELEASE;
		Transform GetTransform(DxGraphic& graphic) NOEXCEPTRELEASE;

		static std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBufferMatrix;
		const Drawable* parent = nullptr;
	};
}