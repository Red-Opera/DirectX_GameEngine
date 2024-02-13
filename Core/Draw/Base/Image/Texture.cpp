#include "stdafx.h"
#include "Texture.h"

#include "../Material.h"

#include "Core/Exception/GraphicsException.h"

Texture::Texture(DxGraphic& graphic, const Material& material)
{
	CREATEINFOMANAGER(graphic);

	// 2D �ؽ�ó�� �������� ���� Desc
	D3D11_TEXTURE2D_DESC textureDesc = { };
	textureDesc.Width = material.GetWidth();	    // ���͸����� �ʺ� ������
	textureDesc.Height = material.GetHeigth();	    // ���͸����� ���̸� ������
	textureDesc.MipLevels = 1;					    // �Ӹ� ���� ����
	textureDesc.ArraySize = 1;					    // �̹��� ũ��
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// �ؽ�ó�� �� �ȼ� ����
	
    // MSAA�� ����� ���
    if (graphic.GetMsaaUsage())
    {
        textureDesc.SampleDesc.Count = 4;             
        textureDesc.SampleDesc.Quality = graphic.GetMsaaQuality() - 1; 
    }

    // MSAA�� ������� ���� ���
    else
    {
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
    }

    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = { };
    initData.pSysMem = material.get();
    initData.SysMemPitch = material.GetWidth() * sizeof(Material::Color);

    // D3D11_TEXTURE2D_DESC�� ���ؼ� Texture2D�� ������
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&textureDesc, &initData, &texture));

    // �̹��� �����͸� ID3D11Texture2D�� �����ϱ� ���� ���̴� ���ҽ� �並 �����ϱ� ���� Desc
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = { };
    viewDesc.Format = textureDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // �̹����� 2D�� ������
    viewDesc.Texture2D.MostDetailedMip = 0;
    viewDesc.Texture2D.MipLevels = 1;

    GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateShaderResourceView(texture.Get(), &viewDesc, &textureView));
}

void Texture::PipeLineSet(DxGraphic& graphic) noexcept
{
    GetDeviceContext(graphic)->PSSetShaderResources(0, 1, textureView.GetAddressOf());
}