#pragma once

#include "ConstantBuffer.h"

#include "Core/RenderingPipeline/Render.h"


class Camera;

namespace Graphic
{
	class ShadowCameraConstantBuffer : public Render
	{
	public:
		ShadowCameraConstantBuffer(UINT slot = 1u);

		void Update();
		void SetCamera(const Camera* camera) noexcept;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	protected:
		struct Transform { DirectX::XMMATRIX viewProjection; };

	private:
		std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBuffer;
		const Camera* camera = nullptr;
	};
}