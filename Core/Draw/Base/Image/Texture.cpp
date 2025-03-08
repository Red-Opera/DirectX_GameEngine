#include "stdafx.h"
#include "Texture.h"

#include "Image.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"


std::unordered_map<std::string, std::unique_ptr<GraphicResource::Image>> Graphic::Texture::imageCache;

namespace Graphic
{
    Texture::Texture(const std::string& path, UINT slot) : slot(slot), path(path)
    {
        CREATEINFOMANAGER(Window::GetDxGraphic());

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

        // 2D �ؽ�ó�� �������� ���� Desc
        D3D11_TEXTURE2D_DESC textureDesc = { };
        textureDesc.Width = image->GetWidth();	    // ���͸����� �ʺ� ������
        textureDesc.Height = image->GetHeight();	    // ���͸����� ���̸� ������
        textureDesc.MipLevels = 0;					    // �Ӹ� ���� ����
        textureDesc.ArraySize = 1;					    // �̹��� ũ��
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// �ؽ�ó�� �� �ȼ� ����

        // MSAA�� ����� ���
        if (Window::GetDxGraphic().GetMsaaUsage())
        {
            textureDesc.SampleDesc.Count = 4;
            textureDesc.SampleDesc.Quality = Window::GetDxGraphic().GetMsaaQuality() - 1;
        }

        // MSAA�� ������� ���� ���
        else
        {
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
        }

        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        // D3D11_TEXTURE2D_DESC�� ���ؼ� Texture2D�� ������
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        GRAPHIC_THROW_INFO(GetDevice(Window::GetDxGraphic())->CreateTexture2D(&textureDesc, nullptr, &texture));

		// ���͸����� ������ �ؽ�ó�� ������Ʈ
		GetDeviceContext(Window::GetDxGraphic())->UpdateSubresource(texture.Get(), 0u, nullptr, image->GetConst(), image->GetWidth() * sizeof(GraphicResource::Image::Color), 0u);

        // �̹��� �����͸� ID3D11Texture2D�� �����ϱ� ���� ���̴� ���ҽ� �並 �����ϱ� ���� Desc
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = { };
        viewDesc.Format = textureDesc.Format;
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // �̹����� 2D�� ������
        viewDesc.Texture2D.MostDetailedMip = 0;
        viewDesc.Texture2D.MipLevels = -1;

        hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(texture.Get(), &viewDesc, &textureView);
        GRAPHIC_THROW_INFO(hr);

		// �Ӹ� ����
		GetDeviceContext(Window::GetDxGraphic())->GenerateMips(textureView.Get());
    }

    void Texture::SetRenderPipeline() NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetShaderResources(slot, 1u, textureView.GetAddressOf()));
    }

    std::shared_ptr<Texture> Texture::GetRender(const std::string& path, UINT slot)
    {
        return RenderManager::GetRender<Texture>(path, slot);
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