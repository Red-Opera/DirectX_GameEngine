#include "stdafx.h"
#include "RenderTarget.h"

#include "Core/Draw/Base/Image/Image.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"

#include <array>

namespace Graphic
{
    RenderTarget::RenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture)
    {
        CREATEINFOMANAGERNOHR(graphic);

        D3D11_TEXTURE2D_DESC textureDESC;
        texture->GetDesc(&textureDESC);
        width = textureDESC.Width;
        height = textureDESC.Height;

        // 렌더링 결과를 출력하기 위하기 위해 생성
        D3D11_RENDER_TARGET_VIEW_DESC renerTargetDesc = {};
        renerTargetDesc.Format = textureDESC.Format;
        renerTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renerTargetDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateRenderTargetView(texture, &renerTargetDesc, &targetView));
        
    }

    RenderTarget::RenderTarget(DxGraphic& graphic, UINT width, UINT height) : width(width), height(height)
    {
        CREATEINFOMANAGERNOHR(graphic);

        // 깊이 스텐실 버퍼의 텍스쳐 제작
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

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
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&textureDesc, nullptr, &texture));

        // 렌더링 결과를 출력하기 위하기 위해 생성
        D3D11_RENDER_TARGET_VIEW_DESC renerTargetDesc = {};
        renerTargetDesc.Format = textureDesc.Format;
        renerTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renerTargetDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateRenderTargetView(texture.Get(), &renerTargetDesc, &targetView));
    }

    void RenderTarget::RenderAsBuffer(DxGraphic& graphic, ID3D11DepthStencilView* depthStencilView) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(graphic);
        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->OMSetRenderTargets(1, targetView.GetAddressOf(), depthStencilView));

        D3D11_VIEWPORT viewport;
        viewport.Width = (float)width;
        viewport.Height = (float)height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->RSSetViewports(1u, &viewport));
    }

    void RenderTarget::RenderAsBuffer(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        ID3D11DepthStencilView* const null = nullptr;
        RenderAsBuffer(graphic, null);
    }

    void RenderTarget::RenderAsBuffer(DxGraphic& graphic, BufferResource* bufferResource) NOEXCEPTRELEASE
    {
        assert(dynamic_cast<DepthStencil*>(bufferResource) != nullptr);

        RenderAsBuffer(graphic, static_cast<DepthStencil*>(bufferResource));
    }

    void RenderTarget::RenderAsBuffer(DxGraphic& graphic, DepthStencil* depthStencil) NOEXCEPTRELEASE
    {
        RenderAsBuffer(graphic, depthStencil ? depthStencil->depthStencilView.Get() : nullptr);
    }

    void RenderTarget::Clear(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        Clear(graphic, { 0.0f, 0.0f, 0.0f, 0.0f });
    }

    void RenderTarget::Clear(DxGraphic& graphic, const std::array<float, 4>& color) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(graphic);

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->ClearRenderTargetView(targetView.Get(), color.data()));
    }

    UINT RenderTarget::GetWidth() const noexcept
    {
        return width;
    }

    UINT RenderTarget::GetHeight() const noexcept
    {
        return height;
    }

    ShaderInputRenderTarget::ShaderInputRenderTarget(DxGraphic& graphic, UINT width, UINT height, UINT slot)
        : RenderTarget(graphic, width, height), slot(slot)
    {
        CREATEINFOMANAGERNOHR(graphic);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        targetView->GetResource(&resource);

        D3D11_SHADER_RESOURCE_VIEW_DESC resourceDESC = {};
        resourceDESC.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        resourceDESC.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        resourceDESC.Texture2D.MostDetailedMip = 0;
        resourceDESC.Texture2D.MipLevels = 1;

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateShaderResourceView(resource.Get(), &resourceDESC, &shaderResourceView));
    }

    void ShaderInputRenderTarget::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(graphic);

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf()));
    }

    GraphicResource::Image ShaderInputRenderTarget::ToImage(DxGraphic& graphic) const
    {
        CREATEINFOMANAGER(graphic);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        shaderResourceView->GetResource(&resource);

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        resource.As(&texture);

        D3D11_TEXTURE2D_DESC textureDESC;
        texture->GetDesc(&textureDESC);
        textureDESC.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        textureDESC.Usage = D3D11_USAGE_STAGING;
        textureDESC.BindFlags = 0;
        
        Microsoft::WRL::ComPtr<ID3D11Texture2D> textureTemp;

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&textureDESC, nullptr, &textureTemp));
        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->CopyResource(textureTemp.Get(), texture.Get()));

        const auto width = GetWidth();
        const auto height = GetHeight();

        GraphicResource::Image image(width, height);
        D3D11_MAPPED_SUBRESOURCE mapped = { };
        GRAPHIC_THROW_INFO(GetDeviceContext(graphic)->Map(textureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped));

        auto mappedBytes = static_cast<const char*>(mapped.pData);

        for (UINT y = 0; y < height; y++)
        {
            auto color = reinterpret_cast<const GraphicResource::Image::Color*>(mappedBytes + mapped.RowPitch * size_t(y));

            for (UINT x = 0; x < width; x++)
                image.SetColorPixel(x, y, *(color + x));
        }

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->Unmap(textureTemp.Get(), 0));

        return image;
    }

    void OutputOnlyRenderTarget::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        assert("OutputOnlyRenderTarget은 Shader Input이 있을 때만 사용할 수 있음" && false);
    }

    OutputOnlyRenderTarget::OutputOnlyRenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture)
        : RenderTarget(graphic, texture)
    {

    }
}