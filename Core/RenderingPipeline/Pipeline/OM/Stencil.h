#pragma once

#include "../Core/RenderingPipeline/Render.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/Window.h"

namespace Graphic
{
	class Stencil : public Render
	{
	public:
		enum class DrawMode { Off, Write, Mask, DepthOff, DepthReversed, DepthFirst };

		Stencil(DrawMode drawMode) : drawMode(drawMode)
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

			else if (drawMode == DrawMode::DepthOff)
			{
				depthStencilDESC.DepthEnable = FALSE;
				depthStencilDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			}

			else if (drawMode == DrawMode::DepthReversed)
				depthStencilDESC.DepthFunc = D3D11_COMPARISON_GREATER;

			else if (drawMode == DrawMode::DepthFirst)
			{
				depthStencilDESC.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
				depthStencilDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

			}

			GetDevice(Window::GetDxGraphic())->CreateDepthStencilState(&depthStencilDESC, &stencil);
		}

		// Render을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->OMSetDepthStencilState(stencil.Get(), 0xFF));
		}

		static std::shared_ptr<Stencil> GetRender(DrawMode drawMode)
		{
			return RenderManager::GetRender<Stencil>(drawMode);
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

				case Graphic::Stencil::DrawMode::DepthOff:
					return "DepthOff"s;

				case Graphic::Stencil::DrawMode::DepthReversed:
					return "DepthReversed"s;

				case Graphic::Stencil::DrawMode::DepthFirst:
					return "DepthFirst"s;
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