#pragma once

#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Vertex.h"

namespace Graphic
{
	class InputLayout : public Render
	{
	public:
		InputLayout(DxGraphic& graphic, VertexCore::VertexLayout vertexLayout, ID3DBlob* shaderCode);

		// Bindable을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		static std::shared_ptr<InputLayout> GetRender(DxGraphic& graphic, const VertexCore::VertexLayout& vertexLayout, ID3DBlob* shaderCode);
		static std::string CreateID(const VertexCore::VertexLayout& vertexLayout, ID3DBlob* shaderCode = nullptr);
		std::string GetID() const noexcept override;
		const VertexCore::VertexLayout GetVertexLayout() const noexcept;

	protected:
		VertexCore::VertexLayout vertexLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};
}