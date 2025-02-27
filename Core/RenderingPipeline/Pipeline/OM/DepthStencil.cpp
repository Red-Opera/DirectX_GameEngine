#include "stdafx.h"
#include "DepthStencil.h"

#include "Core/DxGraphic.h"
#include "Core/Draw/Base/Image/Image.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderTarget.h"

#include "Utility/Cnpy/cnpy.h"

#include <stdexcept>

namespace Graphic
{
    DXGI_FORMAT GetUsageTypeless(DepthStencil::Usage usage)
    {
        switch (usage)
        {
        case Graphic::DepthStencil::Usage::DepthStencil:
            return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;

        case Graphic::DepthStencil::Usage::ShadowDepth:
            return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;

            throw std::runtime_error("해당 DepthStencil의 사용 여부는 존재하지 않음");
        }

        return DXGI_FORMAT::DXGI_FORMAT_YUY2;
    }

    DXGI_FORMAT GetUsageType(DepthStencil::Usage usage)
    {
        switch (usage)
        {
        case Graphic::DepthStencil::Usage::DepthStencil:
            return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;

        case Graphic::DepthStencil::Usage::ShadowDepth:
            return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;

            throw std::runtime_error("해당 DepthStencil의 사용 여부는 존재하지 않음");
        }

        return DXGI_FORMAT::DXGI_FORMAT_YUY2;
    }

    DXGI_FORMAT GetUsageColorType(DepthStencil::Usage usage)
    {
        switch (usage)
        {
        case Graphic::DepthStencil::Usage::DepthStencil:
            return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

        case Graphic::DepthStencil::Usage::ShadowDepth:
            return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;

            throw std::runtime_error("해당 DepthStencil의 사용 여부는 존재하지 않음");
        }

        return DXGI_FORMAT::DXGI_FORMAT_YUY2;
    }

    DepthStencil::DepthStencil(DxGraphic& graphic, UINT width, UINT height, bool canRenderShaderInput, Usage usage)
        : width(width), height(height)
    {
        CREATEINFOMANAGERNOHR(graphic);

        // 깊이 스텐실 버퍼의 텍스쳐 제작
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;

        D3D11_TEXTURE2D_DESC descDepth = {};
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = GetUsageTypeless(usage);       // 깊이 정보를 담는 32비트 float형 데이터로 설정
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;

        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canRenderShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&descDepth, nullptr, &depthStencil));

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDESC;
        depthStencilViewDESC.Format = GetUsageType(usage);
        depthStencilViewDESC.Flags = 0;
        depthStencilViewDESC.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDESC.Texture2D.MipSlice = 0;

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDESC, &depthStencilView));
    }

    DepthStencil::DepthStencil(DxGraphic& graphic, Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, UINT face)
    {
        CREATEINFOMANAGER(graphic);

        D3D11_TEXTURE2D_DESC textureDESC = { };
		texture->GetDesc(&textureDESC);
		width = textureDESC.Width;
		height = textureDESC.Height;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDESC;
		depthStencilViewDESC.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDESC.Flags = 0;
		depthStencilViewDESC.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		depthStencilViewDESC.Texture2D.MipSlice = 0;
		depthStencilViewDESC.Texture2DArray.ArraySize = 1;
		depthStencilViewDESC.Texture2DArray.FirstArraySlice = face;

		hr = GetDevice(graphic)->CreateDepthStencilView(texture.Get(), &depthStencilViewDESC, &depthStencilView);
		GRAPHIC_THROW_INFO(hr);
    }

    std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> DepthStencil::CreateStaging(DxGraphic& graphic) const
    {
        CREATEINFOMANAGER(graphic);

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDESC{ };
        depthStencilView->GetDesc(&depthStencilViewDESC);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        depthStencilView->GetResource(&resource);

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        resource.As(&texture);

        D3D11_TEXTURE2D_DESC textureDESC;
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

        if (depthStencilViewDESC.ViewDimension == D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
        {
            GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->CopySubresourceRegion(textureTemp.Get(), 0, 0, 0, 0, texture.Get(), depthStencilViewDESC.Texture2DArray.FirstArraySlice, nullptr));
        }

        else
        {
            GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->CopyResource(textureTemp.Get(), texture.Get()));
        }

        return { std::move(textureTemp), textureDESC };
    }

    void DepthStencil::RenderAsBuffer(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(graphic);

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->OMSetRenderTargets(0, nullptr, depthStencilView.Get()));
    }

    void DepthStencil::RenderAsBuffer(DxGraphic& graphic, BufferResource* bufferResource) NOEXCEPTRELEASE
    {
        assert(dynamic_cast<RenderTarget*>(bufferResource) != nullptr);
        RenderAsBuffer(graphic, static_cast<RenderTarget*>(bufferResource));
    }

    void DepthStencil::RenderAsBuffer(DxGraphic& graphic, RenderTarget* renderTarget) NOEXCEPTRELEASE
    {
        renderTarget->RenderAsBuffer(graphic, this);
    }

    GraphicResource::Image DepthStencil::ToImage(DxGraphic& graphic, bool linearlize) const
    {
        CREATEINFOMANAGER(graphic);
        auto [textureTemp, textureDESC] = CreateStaging(graphic);

        const auto width = GetWidth();
        const auto height = GetHeight();
        GraphicResource::Image image{ width, height };

        D3D11_MAPPED_SUBRESOURCE mapped = { };
        hr = GetDeviceContext(graphic)->Map(textureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
        GRAPHIC_THROW_INFO(hr);

        auto bytes = static_cast<const char*>(mapped.pData);

        for (UINT y = 0; y < height; y++)
        {
            struct Pixel { char data[4]; };

            auto row = reinterpret_cast<const Pixel*>(bytes + mapped.RowPitch * size_t(y));

            for (UINT x = 0; x < width; x++)
            {
                if (textureDESC.Format == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS)
                {
                    const auto raw = 0xFFFFFF & *reinterpret_cast<const UINT*>(row + x);

                    if (linearlize)
                    {
                        const auto normalize = (float)raw / (float)0xFFFFFF;
                        const auto linearize = 0.01f / (1.01f - normalize);
                        const auto channel = UCHAR(linearize * 255.0f);

                        image.SetColorPixel(x, y, { channel, channel, channel });
                    }

                    else
                    {
                        const UCHAR channel = raw >> 16;
                         
                        image.SetColorPixel(x, y, { channel, channel, channel });
                    }
                }

                else if (textureDESC.Format == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
                {
                    const auto raw = *reinterpret_cast<const float*>(row + x);

                    if (linearlize)
                    {
                        const auto linearize = 0.01f / (1.01f - raw);
                        const auto channel = UCHAR(linearize * 255.0f);

                        image.SetColorPixel(x, y, { channel, channel, channel });
                    }

                    else
                    {
                        const UCHAR channel = UCHAR(raw * 255.0f);

                        image.SetColorPixel(x, y, { channel, channel, channel });
                    }
                }

                else
                    throw std::runtime_error("해당 DXGI_FORMAT 포맷은 Image에 저장할 수 없음");
            }
        }

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->Unmap(textureTemp.Get(), 0));

        return image;
    }

    void DepthStencil::CreateDumpy(DxGraphic& graphic, const std::string& path) const
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

        if (textureDESC.Format != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
            throw std::runtime_error{ "더미 값으로 저장하기 위한 Depth Stencil 버퍼로 사용하는 포맷이 DXGI_FORMAT_R32_TYPELESS가 아님" };

        for (UINT y = 0; y < height; y++)
        {
            auto row = reinterpret_cast<const float*>(bytes + mapped.RowPitch * size_t(y));

            for (UINT x = 0; x < width; x++)
                arr.push_back(row[x]);
        }

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->Unmap(textureTemp.Get(), 0));

        cnpy::npy_save(path, arr.data(), { height, width });
    }

    UINT DepthStencil::GetWidth() const
    {
        return width;
    }

    UINT DepthStencil::GetHeight() const
    {
        return height;
    }

    void DepthStencil::Clear(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        GetDeviceContext(graphic)->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
    }

    ShaderInputDepthStencil::ShaderInputDepthStencil(DxGraphic& graphic, UINT slot, Usage usage)
        : ShaderInputDepthStencil(graphic, graphic.GetWidth(), graphic.GetHeight(), slot, usage)
    {

    }

    ShaderInputDepthStencil::ShaderInputDepthStencil(DxGraphic& graphic, UINT width, UINT height, UINT slot, Usage usage)
        : DepthStencil(graphic, width, height, true, usage), slot(slot)
    {
        CREATEINFOMANAGERNOHR(graphic);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        depthStencilView->GetResource(&resource);

        D3D11_SHADER_RESOURCE_VIEW_DESC resourceDESC = {};
        resourceDESC.Format = GetUsageColorType(usage);
        resourceDESC.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        resourceDESC.Texture2D.MostDetailedMip = 0;
        resourceDESC.Texture2D.MipLevels = 1;

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateShaderResourceView(resource.Get(), &resourceDESC, &shaderResourceView));
    }

    void ShaderInputDepthStencil::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        CREATEINFOMANAGERNOHR(graphic);

        GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetShaderResources(slot, 1u, shaderResourceView.GetAddressOf()));
    }

    OutputOnlyDepthStencil::OutputOnlyDepthStencil(DxGraphic& graphic)
        : OutputOnlyDepthStencil(graphic, graphic.GetWidth(), graphic.GetHeight())
    {

    }

    OutputOnlyDepthStencil::OutputOnlyDepthStencil(DxGraphic& graphic, UINT width, UINT height)
        : DepthStencil(graphic, width, height, false, Usage::DepthStencil)
    {

    }

    OutputOnlyDepthStencil::OutputOnlyDepthStencil(DxGraphic& grapphic, Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, UINT face)
		: DepthStencil(grapphic, std::move(texture), face)
    {
    }

    void OutputOnlyDepthStencil::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        assert("OutputOnlyDepthStencil은 Shader Input이 있을 때만 사용할 수 있음" && false);
    }
}