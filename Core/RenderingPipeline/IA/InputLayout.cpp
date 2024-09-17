#include "stdafx.h"
#include "InputLayout.h"

#include "../RenderManager.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Vertex.h"

using namespace std;

namespace Graphic
{
	InputLayout::InputLayout(DxGraphic& graphic, VertexCore::VertexLayout vertexLayout, ID3DBlob* shaderCode) : vertexLayout(std::move(vertexLayout))
	{
		CREATEINFOMANAGER(graphic);
		const auto inputElement = this->vertexLayout.GetInputElement();

		hr = GetDevice(graphic)->CreateInputLayout(
			inputElement.data(),
			(UINT)inputElement.size(),
			shaderCode->GetBufferPointer(),
			shaderCode->GetBufferSize(),
			&inputLayout);

		GRAPHIC_THROW_INFO(hr);
	}

	void InputLayout::PipeLineSet(DxGraphic& graphic) noexcept
	{
		GetDeviceContext(graphic)->IASetInputLayout(inputLayout.Get());
	}

	std::shared_ptr<InputLayout> InputLayout::GetRender(DxGraphic& graphic, const VertexCore::VertexLayout& vertexLayout, ID3DBlob* shaderCode)
	{
		return RenderManager::GetRender<InputLayout>(graphic, vertexLayout, shaderCode);
	}

	std::string InputLayout::CreateID(const VertexCore::VertexLayout& vertexLayout, ID3DBlob* shaderCode)
	{
		using namespace std::string_literals;

		return typeid(InputLayout).name() + "#"s + vertexLayout.GetCode();
	}

	std::string InputLayout::GetID() const noexcept
	{
		return CreateID(vertexLayout);
	}
}