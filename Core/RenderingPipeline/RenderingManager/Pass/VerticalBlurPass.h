#pragma once

#include "PostProcessFullScreenRenderPass.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"

class DxGraphic;

namespace Graphic
{
	class PixelShader;
	class RenderTarget;
}

namespace RenderGraphNameSpace
{
	class VerticalBlurPass : public PostProcessFullScreenRenderPass
	{
	public:
		VerticalBlurPass(std::string name);

		void Execute() const NOEXCEPTRELEASE override;

	private:
		std::shared_ptr<Graphic::CachingPixelConstantBufferEx> direction;
	};
}