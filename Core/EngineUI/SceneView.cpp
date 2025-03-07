#include "stdafx.h"
#include "SceneView.h"

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/RenderTarget.h"

namespace Graphic
{
    SceneView::SceneView(DxGraphic& graphic, UINT width, UINT height)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = 1000;
        desc.Height = 1000;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        GetDevice(graphic)->CreateTexture2D(&desc, nullptr, &renderTargetTexture);

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = desc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        GetDevice(graphic)->CreateRenderTargetView(renderTargetTexture.Get(), &rtvDesc, &renderTarget);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        GetDevice(graphic)->CreateShaderResourceView(renderTargetTexture.Get(), &srvDesc, &renderTargetView);

        // ����̽� ���ؽ�Ʈ ��������
        auto pContext = GetDeviceContext(graphic);

        // �� ���� ���� Ÿ���� ����
        ID3D11RenderTargetView* rtView = renderTarget.Get();
        pContext->OMSetRenderTargets(1, &rtView, nullptr);

        // ����Ʈ ����
        D3D11_VIEWPORT vp = {};
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = 1000.0f;  // ���� ������ �ؽ�ó ũ��
        vp.Height = 1000.0f;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        pContext->RSSetViewports(1, &vp);

        // ��� Clear
        const float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
        pContext->ClearRenderTargetView(rtView, clearColor);
    }

    void SceneView::Render(DxGraphic& graphic)
    {
        auto pContext = GetDeviceContext(graphic);

        // SceneView�� ���� Ÿ�� ����
        ID3D11RenderTargetView* rtView = renderTarget.Get();
        pContext->OMSetRenderTargets(1, &rtView, nullptr);

        // ���� ImGui�� ȭ�鿡 ǥ��
        ImGui::Begin("Game Window");
        ImGui::Image((ImTextureID)(intptr_t)renderTargetView.Get(), ImVec2(1000.0f, 1000.0f));
        ImGui::End();
    }

    void SceneView::SetRenderPipeline(DxGraphic& graphic)
    {
        
    }
}