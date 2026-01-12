#include "stdafx.h"
#include "NullPixelShader.h"

#include "Core/Window.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
	NullPixelShader::NullPixelShader()
	{

	}

	std::shared_ptr<NullPixelShader> NullPixelShader::GetRender()
	{
		return RenderManager::GetRender<NullPixelShader>();
	}

	std::string NullPixelShader::CreateID()
	{
		return typeid(NullPixelShader).name();
	}

	std::string NullPixelShader::GetID() const noexcept
	{
		return CreateID();
	}

	void NullPixelShader::SetRenderPipeline() NOEXCEPTRELEASE
	{
		Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->PSSetShader(nullptr, nullptr, 0u); }, ErrorCode::GRAPHICS_BindFailed, "널 픽셀 셰이더를 픽셀 셰이더 단계에 바인딩 실패");
	}

}
