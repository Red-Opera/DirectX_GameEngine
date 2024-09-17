#include "stdafx.h"
#include "Texture.h"

#include "../Material.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
    Texture::Texture(DxGraphic& graphic, const std::string& path, UINT slot) : slot(slot), path(path)
    {
        CREATEINFOMANAGER(graphic);
        const auto material = Material::FromFile(path);

        // 2D 텍스처를 가져오기 위한 Desc
        D3D11_TEXTURE2D_DESC textureDesc = { };
        textureDesc.Width = material.GetWidth();	    // 머터리얼의 너비를 가져옴
        textureDesc.Height = material.GetHeigth();	    // 머터리얼의 높이를 가져옴
        textureDesc.MipLevels = 1;					    // 밉맵 개수 설정
        textureDesc.ArraySize = 1;					    // 이미지 크기
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// 텍스처의 한 픽셀 정보

        // MSAA를 사용할 경우
        if (graphic.GetMsaaUsage())
        {
            textureDesc.SampleDesc.Count = 4;
            textureDesc.SampleDesc.Quality = graphic.GetMsaaQuality() - 1;
        }

        // MSAA를 사용하지 않을 경우
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

        // D3D11_TEXTURE2D_DESC를 통해서 Texture2D를 생성함
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&textureDesc, &initData, &texture));

        // 이미지 데이터를 ID3D11Texture2D에 저장하기 위한 셰이더 리소스 뷰를 저장하기 위한 Desc
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = { };
        viewDesc.Format = textureDesc.Format;
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // 이미지를 2D로 보여줌
        viewDesc.Texture2D.MostDetailedMip = 0;
        viewDesc.Texture2D.MipLevels = 1;

        hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), &viewDesc, &textureView);
        GRAPHIC_THROW_INFO(hr);
    }

    void Texture::PipeLineSet(DxGraphic& graphic) noexcept
    {
        GetDeviceContext(graphic)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf());
    }

    std::shared_ptr<Texture> Texture::GetRender(DxGraphic& graphic, const std::string& path, UINT slot)
    {
        return RenderManager::GetRender<Texture>(graphic, path, slot);
    }

    std::string Texture::CreateID(const std::string& path, UINT slot)
    {
        using namespace std::string_literals;

        return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
    }

    std::string Texture::GetID() const noexcept
    {
        return CreateID(path, slot);
    }
}