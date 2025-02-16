#pragma once

#include "../Core/RenderingPipeline/Render.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"

namespace Graphic
{
	class Stencil : public Render
	{
	public:
		enum class DrawMode { Off, Write, Mask };

		Stencil(DxGraphic& graphic, DrawMode drawMode) : drawMode(drawMode)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDESC = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT() };

			if (drawMode == DrawMode::Write)
			{
				depthStencilDESC.DepthEnable = FALSE;
				depthStencilDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				depthStencilDESC.StencilEnable = TRUE;
				depthStencilDESC.StencilWriteMask = 0xFF;
				depthStencilDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			}

			else if (drawMode == DrawMode::Mask)
			{
				depthStencilDESC.DepthEnable = FALSE;
				depthStencilDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				depthStencilDESC.StencilEnable = TRUE;
				depthStencilDESC.StencilWriteMask = 0xFF;
				depthStencilDESC.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				depthStencilDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			}

			GetDevice(graphic)->CreateDepthStencilState(&depthStencilDESC, &stencil);
		}

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(graphic);

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->OMSetDepthStencilState(stencil.Get(), 0xFF));
		}

		static std::shared_ptr<Stencil> GetRender(DxGraphic& graphic, DrawMode drawMode)
		{
			return RenderManager::GetRender<Stencil>(graphic, drawMode);
		}

		static std::string CreateID(DrawMode drawMode)
		{
			using namespace std::string_literals;

			const auto GetDrawModeString = [drawMode]()
			{
				switch (drawMode)
				{
				case Graphic::Stencil::DrawMode::Off:
					return "OFF"s;

				case Graphic::Stencil::DrawMode::Write:
					return "Write"s;

				case Graphic::Stencil::DrawMode::Mask:
					return "Mask"s;
				}

				return "ERROR"s;
			};

			return typeid(Stencil).name() + "#"s + GetDrawModeString();
		}

		std::string GetID() const noexcept override { return CreateID(drawMode); }

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencil;
		DrawMode drawMode;
	};
}