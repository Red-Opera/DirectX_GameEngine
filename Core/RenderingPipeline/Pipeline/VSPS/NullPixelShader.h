#pragma once

#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class NullPixelShader : public Render
	{
	public:
		NullPixelShader(DxGraphic& graphic);

		static std::shared_ptr<NullPixelShader> GetRender(DxGraphic& graphic);
		static std::string CreateID();
		std::string GetID() const noexcept override;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;
	};
}