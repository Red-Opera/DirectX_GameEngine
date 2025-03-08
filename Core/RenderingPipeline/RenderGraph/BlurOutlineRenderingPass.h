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
		BlurOutlineRenderingPass(std::string name, unsigned int width, unsigned int height)
			: RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			renderTarget = std::make_unique<ShaderInputRenderTarget>(width / 2, height / 2, 0);

			AddRender(VertexShader::GetRender("Shader/ColorShader.hlsl"));
			AddRender(PixelShader::GetRender("Shader/ColorShader.hlsl"));
			AddRender(Stencil::GetRender(Stencil::DrawMode::Mask));
			AddRender(ColorBlend::GetRender(false));

			AddDataProvider(DirectRenderPipelineDataProvider<Graphic::RenderTarget>::Create("scratchOut", renderTarget));
		}

		void Execute() const NOEXCEPTRELEASE override
		{
			renderTarget->Clear();

			RenderQueuePass::Execute();
		}
	};
}