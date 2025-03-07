#include "stdafx.h"
#include "PostProcessFullScreenRenderPass.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"

namespace RenderGraphNameSpace
{
	PostProcessFullScreenRenderPass::PostProcessFullScreenRenderPass(const std::string name, DxGraphic& graphic) NOEXCEPTRELEASE
		: RenderingPass(name)
	{
		VertexCore::VertexLayout layout;
		layout.AddType(VertexCore::VertexLayout::Position2D);

		VertexCore::VertexBuffer vertexBufferFull{ layout };
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ -1, 1 });
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ 1, 1 });
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ -1, -1 });
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ 1, -1 });

		AddRender(Graphic::VertexBuffer::GetRender(graphic, "$Full", std::move(vertexBufferFull)));

		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		AddRender(Graphic::IndexBuffer::GetRender(graphic, "$Full", std::move(indices)));

		auto vertexShader = Graphic::VertexShader::GetRender(graphic, "Shader/PostProcessing/ScreenBlur.hlsl");
		AddRender(Graphic::InputLayout::GetRender(graphic, layout, *vertexShader));
		AddRender(std::move(vertexShader));
		AddRender(Graphic::PrimitiveTopology::GetRender(graphic));
		AddRender(Graphic::Rasterizer::GetRender(graphic, false));
	}

	void PostProcessFullScreenRenderPass::Execute(DxGraphic& graphic) const NOEXCEPTRELEASE
	{
		RenderAll(graphic);

		graphic.DrawIndexed(6u);
	}
}