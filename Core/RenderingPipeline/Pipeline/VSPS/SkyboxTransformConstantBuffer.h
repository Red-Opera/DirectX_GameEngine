#pragma once

#include "ConstantBuffer.h"

#include "Core/Draw/Base/Drawable.h"

#include <DirectXMath.h>

class Camera;

namespace Graphic
{
	class SkyboxTransformConstantBuffer : public Render
	{
	public:
		SkyboxTransformConstantBuffer(DxGraphic& graphic, UINT slot = 0u);

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		struct Transform { DirectX::XMMATRIX viewProjection; };

		void UpdateRender(DxGraphic& graphic, const Transform& transform) NOEXCEPTRELEASE;
		Transform GetTransform(DxGraphic& graphic) NOEXCEPTRELEASE;

	private:
		std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBuffer;
	};
}