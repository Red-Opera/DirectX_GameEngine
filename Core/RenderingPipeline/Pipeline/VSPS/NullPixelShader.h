#pragma once

#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class NullPixelShader : public Render
	{
	public:
		NullPixelShader();

		static std::shared_ptr<NullPixelShader> GetRender();
		static std::string CreateID();
		std::string GetID() const noexcept override;

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;
	};
}