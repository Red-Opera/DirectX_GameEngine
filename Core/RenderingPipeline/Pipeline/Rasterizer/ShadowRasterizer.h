#pragma once

#include "Core/RenderingPipeline/Render.h"

#include <array>

namespace Graphic
{
	class ShadowRasterizer : public Render
	{
	public:
		ShadowRasterizer(int depthBias, float slopeBias, float clamp);

		void ChangeDepthBias(int depthBias, float slopeBias, float clamp);

		int GetDepthBias() const;
		float GetSlopeBias() const;
		float GetClamp() const;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

		int depthBias;
		float slopeBias;
		float clamp;
	};
}