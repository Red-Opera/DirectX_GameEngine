#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "../../Pipeline/OM/Stencil.h"
#include "../../Pipeline/Rasterizer/Rasterizer.h"
#include "../../Pipeline/VSPS/NullPixelShader.h"
#include "../../Pipeline/VSPS/VertexShader.h"

#include <vector>

class DxGraphic;

namespace RenderGraphNameSpace
{
	class OutlineMaskPass : public RenderQueuePass
	{
	public:
		OutlineMaskPass(std::string name) : RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<Graphic::DepthStencil>::Create("depthStencil", depthStencil));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(VertexShader::GetRender("Shader/ColorShader.hlsl"));
			AddRender(NullPixelShader::GetRender());
			AddRender(Stencil::GetRender(Stencil::DrawMode::Write));
			AddRender(Rasterizer::GetRender(false));
		}
	};
}