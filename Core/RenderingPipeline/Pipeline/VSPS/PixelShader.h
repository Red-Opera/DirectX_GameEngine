#pragma once
#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class PixelShader : public Render
	{
	public:
		PixelShader(const std::string& path);

		ID3DBlob* GetShaderCode() const noexcept;

		// Bindable��(��) ���� ��ӵ�
		void SetRenderPipeline() NOEXCEPTRELEASE override;

		static std::shared_ptr<PixelShader> GetRender(const std::string& path);
		static std::string CreateID(const std::string& path);
		std::string GetID() const noexcept override;

	protected:
		HRESULT LoadCacheShader(const std::wstring& cacheShaderPath);
		HRESULT SaveCacheShader(const std::wstring& tempShaderPath);
		void GetCompileShader(const std::string& path);

		bool IsShaderCacheVaild(const std::wstring& shaderPath, const std::wstring& cachePath);

		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode;				// �����ϵ� ���̴� �ڵ�
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;		// Pixel Shader
	};
}