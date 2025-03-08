#include "stdafx.h"
#include "PostProcessFullScreenRenderPass.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"

namespace RenderGraphNameSpace
{
	PostProcessFullScreenRenderPass::PostProcessFullScreenRenderPass(const std::string name) NOEXCEPTRELEASE
		: RenderingPass(name)
	{
		VertexCore::VertexLayout layout;
		layout.AddType(VertexCore::VertexLayout::Position2D);

		VertexCore::VertexBuffer vertexBufferFull{ layout };
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ -1, 1 });
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ 1, 1 });
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ -1, -1 });
		vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ 1, -1 });

		AddRender(Graphic::VertexBuffer::GetRender("$Full", std::move(vertexBufferFull)));

		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		AddRender(Graphic::IndexBuffer::GetRender("$Full", std::move(indices)));

		auto vertexShader = Graphic::VertexShader::GetRender("Shader/PostProcessing/ScreenBlur.hlsl");
		AddRender(Graphic::InputLayout::GetRender(layout, *vertexShader));
		AddRender(std::move(vertexShader));
		AddRender(Graphic::PrimitiveTopology::GetRender());
		AddRender(Graphic::Rasterizer::GetRender(false));
	}

	void PostProcessFullScreenRenderPass::Execute() const NOEXCEPTRELEASE
	{
		RenderAll();

		Window::GetDxGraphic().DrawIndexed(6u);
	}
}