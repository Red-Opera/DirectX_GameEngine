#include "stdafx.h"
#include "TextureCube.h"

#include "Base/Image/Image.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include <vector>

namespace Graphic
{
	TextureCube::TextureCube(DxGraphic& graphic, const std::string& path, UINT slot)
		: path(path), slot(slot)
	{
		CREATEINFOMANAGER(graphic);

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
		hr = GetDevice(graphic)->CreateTexture2D(&textureDesc, initData, &texture);

		GRAPHIC_THROW_INFO(hr);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		shaderResourceViewDesc.TextureCube.MipLevels = 1;

		hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, &textureView);
		GRAPHIC_THROW_INFO(hr);
	}

	void TextureCube::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);
		
		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf()));
	}
}