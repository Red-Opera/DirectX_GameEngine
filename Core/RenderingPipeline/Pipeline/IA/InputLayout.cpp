#include "stdafx.h"
#include "InputLayout.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/VertexShader.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/RenderManager.h"

using namespace std;

namespace Graphic
{
	InputLayout::InputLayout(DxGraphic& graphic, VertexCore::VertexLayout vertexLayout, const VertexShader& vertexShader) : vertexLayout(std::move(vertexLayout))
	{
		CREATEINFOMANAGER(graphic);
		const auto inputElement = this->vertexLayout.GetInputElement();
		const auto shaderCode = vertexShader.GetShaderCode();

		hr = GetDevice(graphic)->CreateInputLayout(
			inputElement.data(),
			(UINT)inputElement.size(),
			shaderCode->GetBufferPointer(),
			shaderCode->GetBufferSize(),
			&inputLayout);

		GRAPHIC_THROW_INFO(hr);
	}

	void InputLayout::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->IASetInputLayout(inputLayout.Get()));
	}

	std::shared_ptr<InputLayout> InputLayout::GetRender(DxGraphic& graphic, const VertexCore::VertexLayout& vertexLayout, const VertexShader& vertexShader)
	{
		return RenderManager::GetRender<InputLayout>(graphic, vertexLayout, vertexShader);
	}

	std::string InputLayout::CreateID(const VertexCore::VertexLayout& vertexLayout, const VertexShader& vertexShader)
	{
		using namespace std::string_literals;

		return typeid(InputLayout).name() + "#"s + vertexLayout.GetCode() + "#"s + vertexShader.GetID();
	}

	std::string InputLayout::GetID() const noexcept
	{
		using namespace std::string_literals;

		return typeid(InputLayout).name() + "#"s + vertexLayout.GetCode() + "#"s + vertexShaderID;
	}
	const VertexCore::VertexLayout InputLayout::GetVertexLayout() const noexcept
	{
		return vertexLayout;
	}
}