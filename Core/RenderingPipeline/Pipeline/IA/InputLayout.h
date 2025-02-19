#pragma once

#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Vertex.h"

namespace Graphic
{
	class VertexShader;

	class InputLayout : public Render
	{
	public:
		InputLayout(DxGraphic& graphic, VertexCore::VertexLayout vertexLayout, const VertexShader& vertexShader);

		// Bindable을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		static std::shared_ptr<InputLayout> GetRender(DxGraphic& graphic, const VertexCore::VertexLayout& vertexLayout, const VertexShader& vertexShader);
		static std::string CreateID(const VertexCore::VertexLayout& vertexLayout, const VertexShader& vertexShader);
		std::string GetID() const noexcept override;
		const VertexCore::VertexLayout GetVertexLayout() const noexcept;

	protected:
		VertexCore::VertexLayout vertexLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		std::string vertexShaderID;
	};
}