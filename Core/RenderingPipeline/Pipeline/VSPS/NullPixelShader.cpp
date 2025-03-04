#include "stdafx.h"
#include "NullPixelShader.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
	NullPixelShader::NullPixelShader(DxGraphic& graphic)
	{

	}

	std::shared_ptr<NullPixelShader> NullPixelShader::GetRender(DxGraphic& graphic)
	{
		return RenderManager::GetRender<NullPixelShader>(graphic);
	}

	std::string NullPixelShader::CreateID()
	{
		return typeid(NullPixelShader).name();
	}

	std::string NullPixelShader::GetID() const noexcept
	{
		return CreateID();
	}

	void NullPixelShader::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetShader(nullptr, nullptr, 0u));
	}

}
