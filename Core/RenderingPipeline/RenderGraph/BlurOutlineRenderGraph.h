#pragma once

#include "RenderGraph.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"

#include <memory>

class DxGraphic;

namespace Graphic
{
	class Render;
	class RenderTarget;
}

namespace RenderGraphNameSpace
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(DxGraphic& graphic);
		void RenderWidgets(DxGraphic& graphic);

	private:
		enum class KernelType { Gauss, Box } kernelType = KernelType::Gauss;

		void SetKernelGauss(int radius, float sigma) NOEXCEPTRELEASE;
		void SetKernelBox(int radius) NOEXCEPTRELEASE;

		std::shared_ptr<Graphic::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Graphic::CachingPixelConstantBufferEx> blurDirection;

		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
	};
}