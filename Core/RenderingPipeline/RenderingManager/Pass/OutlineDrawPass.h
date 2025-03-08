#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "../../Pipeline/OM/Stencil.h"
#include "../../Pipeline/Rasterizer/Rasterizer.h"
#include "../../Pipeline/VSPS/PixelShader.h"
#include "../../Pipeline/VSPS/VertexShader.h"

#include <vector>

class DxGraphic;

namespace RenderGraphNameSpace
{
	class OutlineDrawPass : public RenderQueuePass
	{
	public:
		OutlineDrawPass(std::string name) : RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<Graphic::RenderTarget>::Create("renderTarget", renderTarget));
			AddDataConsumer(DirectBufferDataConsumer<Graphic::DepthStencil>::Create("depthStencil", depthStencil));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::RenderTarget>::Create("renderTarget", renderTarget));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(VertexShader::GetRender("Shader/ColorShader.hlsl"));
			AddRender(PixelShader::GetRender("Shader/ColorShader.hlsl"));
			AddRender(Stencil::GetRender(Stencil::DrawMode::Mask));
			AddRender(Rasterizer::GetRender(false));
		}
	};
}