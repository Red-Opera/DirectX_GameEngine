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
		OutlineMaskPass(DxGraphic& graphic, std::string name) : RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<Graphic::DepthStencil>::Create("depthStencil", depthStencil));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl"));
			AddRender(NullPixelShader::GetRender(graphic));
			AddRender(Stencil::GetRender(graphic, Stencil::DrawMode::Write));
			AddRender(Rasterizer::GetRender(graphic, false));
		}
	};
}