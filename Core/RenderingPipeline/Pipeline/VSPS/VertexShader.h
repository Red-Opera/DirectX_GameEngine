#pragma once
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class VertexShader : public Render
	{
	public:
		VertexShader(const std::string& path);

		ID3DBlob* GetShaderCode() const noexcept;

		// Bindable을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override;

		static std::shared_ptr<VertexShader> GetRender(const std::string& path);
		static std::string CreateID(const std::string& path);
		std::string GetID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode;				// 컴파일된 셰이더 코드
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;	// Vertex Shader

		std::string path;
	};
}