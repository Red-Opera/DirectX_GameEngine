#include "stdafx.h"
#include "Rasterizer.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"

namespace Graphic
{
	Rasterizer::Rasterizer(bool isTwoSided) : isTwoSided(isTwoSided)
	{
		CREATEINFOMANAGER(Window::GetDxGraphic());

		// ������ȭ���� �⺻���� ������
		D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterizerDesc.CullMode = isTwoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;			// ����� ������� �ܸ��� ������� ����

		hr = GetDevice(Window::GetDxGraphic())->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

		GRAPHIC_THROW_INFO(hr);
	}

	std::shared_ptr<Rasterizer> Rasterizer::GetRender(bool isTwoSided)
	{
		return RenderManager::GetRender<Rasterizer>(isTwoSided);
	}

	std::string Rasterizer::CreateID(bool isTwoSided)
	{
		using namespace std::literals;

		return typeid(Rasterizer).name() + "#"s + (isTwoSided ? "2s" : "1s");
	}

	std::string Rasterizer::GetID() const noexcept
	{
		return CreateID(isTwoSided);
	}

	void Rasterizer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		// ������ȭ�� ���¸� ������ ������ ���ο� �Է�
		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->RSSetState(rasterizerState.Get()));
	}
}