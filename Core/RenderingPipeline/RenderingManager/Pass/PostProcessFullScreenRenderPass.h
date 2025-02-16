#pragma once

#include "Base/RenderingPass.h"

namespace Graphic
{
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class InputLayout;
}

namespace RenderGraphNameSpace
{
	class PostProcessFullScreenRenderPass : public RenderingPass
	{
	public:
		PostProcessFullScreenRenderPass(const std::string name, DxGraphic& graphic) NOEXCEPTRELEASE;

		// RenderingPass을(를) 통해 상속됨
		void Execute(DxGraphic& graphic) const NOEXCEPTRELEASE override;
	};
}