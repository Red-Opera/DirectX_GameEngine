#pragma once
#include "../Render.h"

#include "ConstantBuffer.h"
#include "Core/Draw/Base/Drawable.h"

namespace Graphic
{
	class TransformConstantBuffer : public Render
	{
	public:
		TransformConstantBuffer(DxGraphic& graphic, const Drawable& parent, UINT slot = 0u);

		// Bindable을(를) 통해 상속됨
		void PipeLineSet(DxGraphic& graphic) noexcept override;

	protected:
		struct Transform
		{
			DirectX::XMMATRIX worldViewProjection;
			DirectX::XMMATRIX transform;
		};

		void UpdatePipeLineSet(DxGraphic& graphic, const Transform& transform) noexcept;
		Transform GetTransform(DxGraphic& graphic) noexcept;

		static std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBufferMatrix;
		const Drawable& parent;
	};
}