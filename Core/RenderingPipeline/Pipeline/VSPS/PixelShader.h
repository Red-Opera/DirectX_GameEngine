#pragma once
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class PixelShader : public Render
	{
	public:
		PixelShader(DxGraphic& graphic, const std::string& path);

		ID3DBlob* GetShaderCode() const noexcept;

		// Bindable��(��) ���� ��ӵ�
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

		static std::shared_ptr<PixelShader> GetRender(DxGraphic& graphic, const std::string& path);
		static std::string CreateID(const std::string& path);
		std::string GetID() const noexcept override;

	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode;				// �����ϵ� ���̴� �ڵ�
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;		// Pixel Shader
	};
}