#include "stdafx.h"
#include "TextureCube.h"

#include "../Base/Image/Image.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"

#include <vector>

namespace Graphic
{
	TextureCube::TextureCube(const std::string& path, UINT slot)
		: path(path), slot(slot)
	{
		CREATEINFOMANAGER(Window::GetDxGraphic());

		std::vector<GraphicResource::Image> images;

		for (int i = 0; i < 6; i++)
			images.push_back(GraphicResource::Image::FromFile(path + "/" + std::to_string(i) + ".png"));

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = images[0].GetWidth();
		textureDesc.Height = images[0].GetHeight();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SUBRESOURCE_DATA initData[6];

		for (int i = 0; i < 6; i++)
		{
			initData[i].pSysMem = images[i].GetConst();
			initData[i].SysMemPitch = images[i].GetBytePitch();
			initData[i].SysMemSlicePitch = 0;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		hr = GetDevice(Window::GetDxGraphic())->CreateTexture2D(&textureDesc, initData, &texture);

		GRAPHIC_THROW_INFO(hr);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		shaderResourceViewDesc.TextureCube.MipLevels = 1;

		hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, &textureView);
		GRAPHIC_THROW_INFO(hr);
	}

	void TextureCube::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());
		
		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetShaderResources(slot, 1u, textureView.GetAddressOf()));
	}

	DepthTextureCube::DepthTextureCube(UINT size, UINT slot)
		: slot(slot)
	{
		CREATEINFOMANAGER(Window::GetDxGraphic());

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = size;
		textureDesc.Height = size;

		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;

		textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		hr = GetDevice(Window::GetDxGraphic())->CreateTexture2D(&textureDesc, nullptr, &texture);
		GRAPHIC_THROW_INFO(hr);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = { };
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, &textureView);
		GRAPHIC_THROW_INFO(hr);

		for (UINT face = 0; face < 6; face++)
			depthStencil.push_back(std::make_shared<OutputOnlyDepthStencil>(texture, face));
	}

	std::shared_ptr<OutputOnlyDepthStencil> DepthTextureCube::GetDepthStencil(size_t index) const
	{
		return depthStencil[index];
	}

	void DepthTextureCube::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetShaderResources(slot, 1u, textureView.GetAddressOf()));
	}

	RenderTargetTextureCube::RenderTargetTextureCube(UINT width, UINT height, UINT slot, DXGI_FORMAT format)
		:slot(slot)
	{
		CREATEINFOMANAGER(Window::GetDxGraphic());

		D3D11_TEXTURE2D_DESC textureDESC = { };
		textureDESC.Width = width;
		textureDESC.Height = height;
		textureDESC.MipLevels = 1;
		textureDESC.ArraySize = 6;
		textureDESC.Format = format;

		textureDESC.SampleDesc.Count = 1;
		textureDESC.SampleDesc.Quality = 0;

		textureDESC.Usage = D3D11_USAGE_DEFAULT;

		textureDESC.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDESC.CPUAccessFlags = 0;
		textureDESC.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		
		hr = GetDevice(Window::GetDxGraphic())->CreateTexture2D(&textureDESC, nullptr, &texture);
		GRAPHIC_THROW_INFO(hr);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDESC = { };
		shaderResourceViewDESC.Format = textureDESC.Format;
		shaderResourceViewDESC.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDESC.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDESC.Texture2D.MipLevels = 1;

		hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), &shaderResourceViewDESC, &textureView);
		GRAPHIC_THROW_INFO(hr);

		for (UINT face = 0; face < 6; face++)
			renderTarget.push_back(std::make_shared<OutputOnlyRenderTarget>(texture.Get(), face));
	}

	std::shared_ptr<OutputOnlyRenderTarget> RenderTargetTextureCube::GetRenderTarget(size_t index) const
	{
		return renderTarget[index];
	}

	void RenderTargetTextureCube::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetShaderResources(slot, 1u, textureView.GetAddressOf()));
	}
}