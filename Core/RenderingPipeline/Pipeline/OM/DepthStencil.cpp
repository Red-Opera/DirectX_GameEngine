#include "stdafx.h"
#include "DepthStencil.h"

#include "Core/Window.h"
#include "Core/DxGraphic.h"
#include "Core/Draw/Base/Image/Image.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderTarget.h"

#include "External/Cnpy/cnpy.h"

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

    DepthStencil::DepthStencil(UINT width, UINT height, bool canRenderShaderInput, Usage usage)
        : width(width), height(height)
    {
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

        HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateTexture2D(&descDepth, nullptr, &depthStencil);
		Require::Check(hr, ErrorCode::GRAPHICS_GetBufferFailed, "해당 설정으로 깊이 스텐실 버퍼 텍스쳐 생성 실패");

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDESC;
        depthStencilViewDESC.Format = GetUsageType(usage);
        depthStencilViewDESC.Flags = 0;
        depthStencilViewDESC.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDESC.Texture2D.MipSlice = 0;

        hr = GetDevice(Window::GetDxGraphic())->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDESC, &depthStencilView);
		Require::Check(hr, ErrorCode::GRAPHICS_GetBufferFailed, "해당 설정으로 깊이 스텐실 뷰 생성 실패");
    }

    DepthStencil::DepthStencil(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, UINT face)
    {
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

		HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateDepthStencilView(texture.Get(), &depthStencilViewDESC, &depthStencilView);
		Require::Check(hr, ErrorCode::GRAPHICS_GetBufferFailed, "해당 설정으로 깊이 스텐실 뷰 생성 실패");
    }

    std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> DepthStencil::CreateStaging() const
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDESC{ };
        depthStencilView->GetDesc(&depthStencilViewDESC);           // 현재 깊이 스텐실 뷰의 설명서를 가져옴

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        depthStencilView->GetResource(&resource);                   // 깊이 스텐실 뷰에 연결된 이미지 데이터를 가져옴

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);                                      // 가져온 이미지 데이터를 리소스를 2D 텍스쳐로 변환

        D3D11_TEXTURE2D_DESC textureDESC;
        texture->GetDesc(&textureDESC);                             // 텍스처의 설명서를 가져옴

        D3D11_TEXTURE2D_DESC tempTextureDESC = textureDESC;
        tempTextureDESC.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tempTextureDESC.Usage = D3D11_USAGE_STAGING;                // CPU가 읽을 수 있는 용도로 설정
        tempTextureDESC.BindFlags = 0;
        tempTextureDESC.MiscFlags = 0;
        tempTextureDESC.ArraySize = 1;

        // CPU가 읽을 수 있는 임시 텍스처 생성
        Microsoft::WRL::ComPtr<ID3D11Texture2D> textureTemp;
        HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateTexture2D(&tempTextureDESC, nullptr, &textureTemp);
		Require::Check(hr, ErrorCode::GRAPHICS_GetBufferFailed, "해당 설정으로 임시 텍스처 생성 실패");

        if (depthStencilViewDESC.ViewDimension == D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
            Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->CopySubresourceRegion(textureTemp.Get(), 0, 0, 0, 0, texture.Get(), depthStencilViewDESC.Texture2DArray.FirstArraySlice, nullptr); }, ErrorCode::GRAPHICS_CopyResourceFailed, "깊이 스텐실 버퍼를 임시 텍스처로 복사하는데 실패");

        else
            Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->CopyResource(textureTemp.Get(), texture.Get()); }, ErrorCode::GRAPHICS_CopyResourceFailed, "깊이 스텐실 버퍼를 임시 텍스처로 복사하는데 실패");
        
        return { std::move(textureTemp), textureDESC };
    }

    void DepthStencil::RenderAsBuffer() NOEXCEPTRELEASE
    {
		Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->OMSetRenderTargets(0, nullptr, depthStencilView.Get()); }, ErrorCode::GRAPHICS_BindFailed, "깊이 스텐실 버퍼를 단독으로 렌더 타겟으로 설정하는데 실패");
    }

    void DepthStencil::RenderAsBuffer(BufferResource* bufferResource) NOEXCEPTRELEASE
    {
        assert(dynamic_cast<RenderTarget*>(bufferResource) != nullptr);
        RenderAsBuffer(static_cast<RenderTarget*>(bufferResource));
    }

    void DepthStencil::RenderAsBuffer(RenderTarget* renderTarget) NOEXCEPTRELEASE
    {
        renderTarget->RenderAsBuffer(this);
    }

    GraphicResource::Image DepthStencil::ToImage(bool linearlize) const
    {
        auto [textureTemp, textureDESC] = CreateStaging();

        const auto width = GetWidth();
        const auto height = GetHeight();
        GraphicResource::Image image{ width, height };

        D3D11_MAPPED_SUBRESOURCE mapped = { };
        HRESULT hr = GetDeviceContext(Window::GetDxGraphic())->Map(textureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
		Require::Check(hr, ErrorCode::GRAPHICS_MapUnmapFailed, "깊이 스텐실 버퍼를 이미지로 변환하기 위해 맵핑하는데 실패");

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

		Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->Unmap(textureTemp.Get(), 0); }, ErrorCode::GRAPHICS_MapUnmapFailed, "깊이 스텐실 버퍼의 맵핑을 해제하는데 실패");

        return image;
    }

    void DepthStencil::CreateDumpy(const std::string& path) const
    {
        auto [textureTemp, textureDESC] = CreateStaging();

        const auto width = GetWidth();
        const auto height = GetHeight();

        std::vector<float> arr;
        arr.reserve(width * height);

        D3D11_MAPPED_SUBRESOURCE mapped = { };
        HRESULT hr = GetDeviceContext(Window::GetDxGraphic())->Map(textureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped);
        
		Require::Check(hr, ErrorCode::GRAPHICS_MapUnmapFailed, "깊이 스텐실 버퍼를 더미 값으로 저장하기 위해 맵핑하는데 실패");

        auto bytes = static_cast<const char*>(mapped.pData);

        if (textureDESC.Format != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
            throw std::runtime_error{ "더미 값으로 저장하기 위한 Depth Stencil 버퍼로 사용하는 포맷이 DXGI_FORMAT_R32_TYPELESS가 아님" };

        for (UINT y = 0; y < height; y++)
        {
            auto row = reinterpret_cast<const float*>(bytes + mapped.RowPitch * size_t(y));

            for (UINT x = 0; x < width; x++)
                arr.push_back(row[x]);
        }

		Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->Unmap(textureTemp.Get(), 0); }, ErrorCode::GRAPHICS_MapUnmapFailed, "깊이 스텐실 버퍼의 맵핑을 해제하는데 실패");

        std::vector<size_t> shape = { static_cast<size_t>(height), static_cast<size_t>(width) };

        cnpy::npy_save(path, arr.data(), shape);
    }

    UINT DepthStencil::GetWidth() const
    {
        return width;
    }

    UINT DepthStencil::GetHeight() const
    {
        return height;
    }

    void DepthStencil::Clear() NOEXCEPTRELEASE
    {
        GetDeviceContext(Window::GetDxGraphic())->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
    }

    ShaderInputDepthStencil::ShaderInputDepthStencil(UINT slot, Usage usage)
        : ShaderInputDepthStencil(Window::GetDxGraphic().GetWidth(), Window::GetDxGraphic().GetHeight(), slot, usage)
    {

    }

    ShaderInputDepthStencil::ShaderInputDepthStencil(UINT width, UINT height, UINT slot, Usage usage)
        : DepthStencil(width, height, true, usage), slot(slot)
    {
        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        depthStencilView->GetResource(&resource);

        D3D11_SHADER_RESOURCE_VIEW_DESC resourceDESC = {};
        resourceDESC.Format = GetUsageColorType(usage);
        resourceDESC.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        resourceDESC.Texture2D.MostDetailedMip = 0;
        resourceDESC.Texture2D.MipLevels = 1;

        HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(resource.Get(), &resourceDESC, &shaderResourceView);
		Require::Check(hr, ErrorCode::GRAPHICS_GetBufferFailed, "해당 설정으로 셰이더 입력용 깊이 스텐실 셰이더 리소스 뷰 생성 실패");
    }

    void ShaderInputDepthStencil::SetRenderPipeline() NOEXCEPTRELEASE
    {
        Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->PSSetShaderResources(slot, 1u, shaderResourceView.GetAddressOf());
			}, ErrorCode::GRAPHICS_BindFailed, "셰이더 입력용 깊이 스텐실을 렌더 파이프라인에 설정하는데 실패");
    }

    OutputOnlyDepthStencil::OutputOnlyDepthStencil()
        : OutputOnlyDepthStencil(Window::GetDxGraphic().GetWidth(), Window::GetDxGraphic().GetHeight())
    {

    }

    OutputOnlyDepthStencil::OutputOnlyDepthStencil(UINT width, UINT height)
        : DepthStencil(width, height, false, Usage::DepthStencil)
    {

    }

    OutputOnlyDepthStencil::OutputOnlyDepthStencil(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, UINT face)
		: DepthStencil(std::move(texture), face)
    {
    }

    void OutputOnlyDepthStencil::SetRenderPipeline() NOEXCEPTRELEASE
    {
        assert("OutputOnlyDepthStencil은 Shader Input이 있을 때만 사용할 수 있음" && false);
    }
}