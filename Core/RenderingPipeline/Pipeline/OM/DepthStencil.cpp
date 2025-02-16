#include "stdafx.h"
#include "DepthStencil.h"

#include "Core/DxGraphic.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderTarget.h"

namespace Graphic
{
    DepthStencil::DepthStencil(DxGraphic& graphic, UINT width, UINT height, bool canRenderShaderInput)
    {
        CREATEINFOMANAGERNOHR(graphic);

        // 깊이 스텐실 버퍼의 텍스쳐 제작
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;

        D3D11_TEXTURE2D_DESC descDepth = {};
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;       // 깊이 정보를 담는 32비트 float형 데이터로 설정
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;

        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canRenderShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateTexture2D(&descDepth, nullptr, &depthStencil));
        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateDepthStencilView(depthStencil.Get(), nullptr, &depthStencilView));
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

    void DepthStencil::Clear(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        GetDeviceContext(graphic)->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
    }

    ShaderInputDepthStencil::ShaderInputDepthStencil(DxGraphic& graphic, UINT slot)
        : DepthStencil(graphic, graphic.GetWidth(), graphic.GetHeight(), slot)
    {

    }
    ShaderInputDepthStencil::ShaderInputDepthStencil(DxGraphic& graphic, UINT width, UINT height, UINT slot)
        : DepthStencil(graphic, width, height, true), slot(slot)
    {
        CREATEINFOMANAGERNOHR(graphic);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        depthStencilView->GetResource(&resource);

        D3D11_SHADER_RESOURCE_VIEW_DESC resourceDESC = {};
        resourceDESC.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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
        : DepthStencil(graphic, width, height, false)
    {

    }

    void OutputOnlyDepthStencil::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
    {
        assert("OutputOnlyDepthStencil은 Shader Input이 있을 때만 사용할 수 있음" && false);
    }
}