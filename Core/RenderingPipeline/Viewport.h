#pragma once

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

namespace Graphic
{
	class Viewport : public Render
	{
	public:
		Viewport(DxGraphic& graphic)
			: Viewport(graphic, (float)graphic.GetWidth(), (float)graphic.GetHeight())
		{

		}

		Viewport(DxGraphic& graphic, float width, float height)
		{
			viewport.Width = width;
			viewport.Height = height;

			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
		}

		// Render��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(graphic);

			GetDeviceContext(graphic)->RSSetViewports(1u, &viewport);
		}

	private:
		D3D11_VIEWPORT viewport{ };
	};
}