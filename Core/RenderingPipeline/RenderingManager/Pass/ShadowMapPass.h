#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"
	
#include "Core/Camera/Camera.h"
#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/NullPixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/VertexShader.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/ShadowRasterizer.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/Rasterizer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"

#include <vector>


class DxGraphic;

namespace RenderGraphNameSpace
{
	class ShadowMapPass : public RenderQueuePass
	{
	public:
		ShadowMapPass(DxGraphic& graphic, std::string name)
			: RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			depthStencil = std::make_unique<ShaderInputDepthStencil>(graphic, 3, DepthStencil::Usage::ShadowDepth);

			AddRender(VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl"));
			AddRender(NullPixelShader::GetRender(graphic));
			AddRender(Stencil::GetRender(graphic, Stencil::DrawMode::Off));
			AddRender(ColorBlend::GetRender(graphic, false));
			AddRender(std::make_shared<Graphic::ShadowRasterizer>(graphic, 50, 2.0f, 0.1f));

			AddDataProvider(DirectRenderPipelineDataProvider<Graphic::DepthStencil>::Create("Map", depthStencil));
		}

		void Execute(DxGraphic& graphic) const NOEXCEPTRELEASE override
		{
			depthStencil->Clear(graphic);

			shadowCamera->RenderToGraphic(graphic);
			RenderQueuePass::Execute(graphic);
		}

		void DumpShadowMap(DxGraphic& graphic, const std::string& path) const
		{
			depthStencil->ToImage(graphic).Save(path);
		}

		void RenderShadowCamera(const Camera& camera) noexcept
		{
			shadowCamera = &camera;
		}

	private:
		const Camera* shadowCamera = nullptr;
	};
}