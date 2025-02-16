#include "stdafx.h"
#include "Texture.h"

#include "Image.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"


std::unordered_map<std::string, std::unique_ptr<GraphicResource::Image>> Graphic::Texture::imageCache;

namespace Graphic
{
    Texture::Texture(DxGraphic& graphic, const std::string& path, UINT slot) : slot(slot), path(path)
    {
        CREATEINFOMANAGER(graphic);

        GraphicResource::Image* image = nullptr;

        auto imageIter = imageCache.find(path);
        if (imageIter != imageCache.end())
            image = imageIter->second.get();

        else
        {
            auto newImage = std::make_unique<GraphicResource::Image>(GraphicResource::Image::FromFile(path));
            imageCache[path] = std::move(newImage);

            image = imageCache[path].get();
        }

        if (image == nullptr)
            return;

        hasAlpha = image->HasAlpha();

        //const auto image = GraphicResource::Image::FromFile(path);
        //hasAlpha = image.HasAlpha();

        // 2D 텍스처를 가져오기 위한 Desc
        D3D11_TEXTURE2D_DESC textureDesc = { };
        textureDesc.Width = image->GetWidth();	    // 머터리얼의 너비를 가져옴
        textureDesc.Height = image->GetHeight();	    // 머터리얼의 높이를 가져옴
        textureDesc.MipLevels = 0;					    // 밉맵 개수 설정
        textureDesc.ArraySize = 1;					    // 이미지 크기
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 텍스처의 한 픽셀 정보

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
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        // D3D11_TEXTURE2D_DESC를 통해서 Texture2D를 생성함
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&textureDesc, nullptr, &texture));

		// 머터리얼의 정보를 텍스처에 업데이트
		GetDeviceContext(graphic)->UpdateSubresource(texture.Get(), 0u, nullptr, image->getConst(), image->GetWidth() * sizeof(GraphicResource::Image::Color), 0u);

        // 이미지 데이터를 ID3D11Texture2D에 저장하기 위한 셰이더 리소스 뷰를 저장하기 위한 Desc
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = { };
        viewDesc.Format = textureDesc.Format;
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // 이미지를 2D로 보여줌
        viewDesc.Texture2D.MostDetailedMip = 0;
        viewDesc.Texture2D.MipLevels = -1;

        hr = GetDevice(graphic)->CreateShaderResourceView(texture.Get(), &viewDesc, &textureView);
        GRAPHIC_THROW_INFO(hr);

		// 밉맵 생성
		GetDeviceContext(graphic)->GenerateMips(textureView.Get());
    }

    void Texture::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(graphic);

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf()));
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

    bool Texture::HasAlpha() const noexcept
    {
        return hasAlpha;
    }

    UINT Texture::CountMipLevels(UINT width, UINT height) noexcept
    {
        const float xSteps = (float)std::ceil(std::log2(width));
        const float ySteps = (float)std::ceil(std::log2(height));

        return (UINT)(std::max)(xSteps, ySteps);
    }
}