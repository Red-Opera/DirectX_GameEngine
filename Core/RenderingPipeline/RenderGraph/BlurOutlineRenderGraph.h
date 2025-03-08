#pragma once

#include "RenderGraph.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"

#include <memory>

class DxGraphic;
class Camera;

namespace Graphic
{
	class Render;
	class RenderTarget;
	class ShadowSamplerState;
	class ShadowRasterizer;
}

namespace RenderGraphNameSpace
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph();
		void RenderWindows();

		void RenderMainCamera(Camera& camera);
		void RenderShadowCamera(Camera& camera);
		void DumpShadowMap(const std::string& path);

	private:
		enum class KernelType { Gauss, Box } kernelType = KernelType::Gauss;

		void SetKernelGauss(int radius, float sigma) NOEXCEPTRELEASE;
		void SetKernelBox(int radius) NOEXCEPTRELEASE;

		void RenderKernelWindow();
		void RenderShadowWindow();

		std::shared_ptr<Graphic::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Graphic::CachingPixelConstantBufferEx> blurDirection;

		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
	};
}