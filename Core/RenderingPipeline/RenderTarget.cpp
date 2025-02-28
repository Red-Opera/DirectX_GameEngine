#include "stdafx.h"
#include "RenderTarget.h"

#include "Core/Draw/Base/Image/Image.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"

#include "External/Cnpy/cnpy.h"

#include <array>
#include <stdexcept>

namespace Graphic
{
    RenderTarget::RenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture, std::optional<UINT> face)
    {
        CREATEINFOMANAGERNOHR(graphic);

        D3D11_TEXTURE2D_DESC textureDESC;
        texture->GetDesc(&textureDESC);
        width = textureDESC.Width;
        height = textureDESC.Height;

        // 렌더링 결과를 출력하기 위하기 위해 생성
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetDESC = {};
        renderTargetDESC.Format = textureDESC.Format;

        if (face.has_value())
        {
            renderTargetDESC.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            renderTargetDESC.Texture2DArray.ArraySize = 1;
            renderTargetDESC.Texture2DArray.FirstArraySlice = *face;
            renderTargetDESC.Texture2DArray.MipSlice = 0;
        }

        else
        {
            renderTargetDESC.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetDESC.Texture2D = D3D11_TEX2D_RTV{ 0 };
        }

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateRenderTargetView(texture, &renderTargetDESC, &targetView));
        
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

    std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> RenderTarget::CreateStaging(DxGraphic& graphic) const
    {
        CREATEINFOMANAGER(graphic);

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDESC{ };
        targetView->GetDesc(&renderTargetViewDESC);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        targetView->GetResource(&resource);

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        resource.As(&texture);

        D3D11_TEXTURE2D_DESC textureDESC{ };
        texture->GetDesc(&textureDESC);

        D3D11_TEXTURE2D_DESC tempTextureDESC = textureDESC;
        tempTextureDESC.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        tempTextureDESC.Usage = D3D11_USAGE_STAGING;
        tempTextureDESC.BindFlags = 0;
        tempTextureDESC.MiscFlags = 0;
        tempTextureDESC.ArraySize = 1;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> textureTemp;
        hr = GetDevice(graphic)->CreateTexture2D(&tempTextureDESC, nullptr, &textureTemp);
        GRAPHIC_THROW_INFO(hr);

        if (renderTargetViewDESC.ViewDimension == D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DARRAY)
        {
            GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->CopySubresourceRegion(textureTemp.Get(), 0, 0, 0, 0, texture.Get(), renderTargetViewDESC.Texture2DArray.FirstArraySlice, nullptr));
        }

        else
        {
            GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->CopyResource(textureTemp.Get(), texture.Get()));
        }

        return { std::move(textureTemp), textureDESC };
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

    GraphicResource::Image RenderTarget::ToImage(DxGraphic& graphic) const
    {
        CREATEINFOMANAGER(graphic);

        auto [textureTemp, textureDESC] = CreateStaging(graphic);

        if (textureDESC.Format != DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM)
            throw std::runtime_error{ "이미지로 저장하기 위한 RenderTarget 버퍼로 사용하는 포맷이 DXGI_FORMAT_R8G8B8A8_UNORM가 아님" };

        const auto width = GetWidth();
        const auto height = GetHeight();

        GraphicResource::Image image{ width, height };
        D3D11_MAPPED_SUBRESOURCE mapped = { };

        hr = GetDeviceContext(graphic)->Map(textureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
        GRAPHIC_THROW_INFO(hr);

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

    void RenderTarget::CreateDumpy(DxGraphic& graphic, const std::string& path) const
    {
        CREATEINFOMANAGER(graphic);

        auto [textureTemp, textureDESC] = CreateStaging(graphic);

        const auto width = GetWidth();
        const auto height = GetHeight();

        std::vector<float> arr;
        arr.reserve(width * height);

        D3D11_MAPPED_SUBRESOURCE mapped = { };
        hr = GetDeviceContext(graphic)->Map(textureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
        GRAPHIC_THROW_INFO(hr);

        auto bytes = static_cast<const char*>(mapped.pData);

        UINT elementCount = 0;

        if (textureDESC.Format == DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT)
        {
            elementCount = 1;

            for (UINT y = 0; y < height; y++)
            {
                auto row = reinterpret_cast<const float*>(bytes + mapped.RowPitch * size_t(y));

                for (UINT x = 0; x < width; x++)
                    arr.push_back(row[x]);
            }
        }
        
        else if (textureDESC.Format == DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT)
        {
            elementCount = 2;

            struct TwoType { float r, g; };

            for (UINT y = 0; y < height; y++)
            {
                auto row = reinterpret_cast<const TwoType*>(bytes + mapped.RowPitch * size_t(y));

                for (UINT x = 0; x < width; x++)
                {
                    arr.push_back(row[x].r);
                    arr.push_back(row[x].g);
                }
            }
        }

        else
        {
            GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->Unmap(textureTemp.Get(), 0));
            throw std::runtime_error{ "더미 값으로 저장하기 위한 RenderTarget 버퍼로 사용하는 포맷이 잘못되었음" };
        }

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->Unmap(textureTemp.Get(), 0));

        cnpy::npy_save(path, arr.data(), { height, width, elementCount });
    }

    void OutputOnlyRenderTarget::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        assert("OutputOnlyRenderTarget은 Shader Input이 있을 때만 사용할 수 있음" && false);
    }

    OutputOnlyRenderTarget::OutputOnlyRenderTarget(DxGraphic& graphic, ID3D11Texture2D* texture, std::optional<UINT> face)
        : RenderTarget(graphic, texture, face)
    {

    }
}