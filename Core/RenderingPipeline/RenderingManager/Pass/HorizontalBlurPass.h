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
	class HorizontalBlurPass : public PostProcessFullScreenRenderPass
	{
	public:
		HorizontalBlurPass(std::string name, unsigned int width, unsigned int height);

		void Execute() const NOEXCEPTRELEASE override;

	private:
		std::shared_ptr<Graphic::CachingPixelConstantBufferEx> direction;
	};
}