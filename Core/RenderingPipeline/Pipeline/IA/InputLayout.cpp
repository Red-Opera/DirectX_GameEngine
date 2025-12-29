#include "stdafx.h"
#include "InputLayout.h"

#include "Core/Window.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/VertexShader.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/RenderManager.h"

using namespace std;

namespace Graphic
{
	InputLayout::InputLayout(VertexCore::VertexLayout vertexLayout, const VertexShader& vertexShader) : vertexLayout(std::move(vertexLayout))
	{
		const auto inputElement = this->vertexLayout.GetInputElement();
		const auto shaderCode = vertexShader.GetShaderCode();

		HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateInputLayout(
			inputElement.data(),
			(UINT)inputElement.size(),
			shaderCode->GetBufferPointer(),
			shaderCode->GetBufferSize(),
			&inputLayout);

		Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "해당 버텍스 레이아웃과 셰이더로 입력 레이아웃 생성 실패");
	}

	void InputLayout::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->IASetInputLayout(inputLayout.Get()));
	}

	std::shared_ptr<InputLayout> InputLayout::GetRender(const VertexCore::VertexLayout& vertexLayout, const VertexShader& vertexShader)
	{
		return RenderManager::GetRender<InputLayout>(vertexLayout, vertexShader);
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