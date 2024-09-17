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

		// Bindable��(��) ���� ��ӵ�
		void PipeLineSet(DxGraphic& graphic) noexcept override;

	private:
		struct Transform
		{
			DirectX::XMMATRIX worldViewProjection;
			DirectX::XMMATRIX transform;
		};

		static std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBufferMatrix;
		const Drawable& parent;
	};
}