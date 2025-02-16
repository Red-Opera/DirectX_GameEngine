#pragma once

#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/VertexShader.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/Rasterizer.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderQueuePass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"

#include <vector>

class DxGrpahic;

namespace RenderGraphNameSpace
{
	class BlurOutlineRenderingPass : public RenderQueuePass
	{
	public:
		BlurOutlineRenderingPass(DxGraphic& graphic, std::string name, unsigned int width, unsigned int height)
			: RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			renderTarget = std::make_unique<ShaderInputRenderTarget>(graphic, width / 2, height / 2, 0);

			AddRender(VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl"));
			AddRender(PixelShader::GetRender(graphic, "Shader/ColorShader.hlsl"));
			AddRender(Stencil::GetRender(graphic, Stencil::DrawMode::Mask));
			AddRender(ColorBlend::GetRender(graphic, false));

			AddDataProvider(DirectRenderPipelineDataProvider<Graphic::RenderTarget>::Create("scratchOut", renderTarget));
		}

		void Execute(DxGraphic& graphic) const NOEXCEPTRELEASE override
		{
			renderTarget->Clear(graphic);

			RenderQueuePass::Execute(graphic);
		}
	};
}