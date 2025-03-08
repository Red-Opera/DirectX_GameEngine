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
		SkyboxTransformConstantBuffer(UINT slot = 0u);

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	protected:
		struct Transform { DirectX::XMMATRIX viewProjection; };

		void UpdateRender(const Transform& transform) NOEXCEPTRELEASE;
		Transform GetTransform() NOEXCEPTRELEASE;

	private:
		std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBuffer;
	};
}