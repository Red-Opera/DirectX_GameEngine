#include "stdafx.h"
#include "SceneView.h"

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/Window.h"

namespace Graphic
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> SceneView::renderTargetTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SceneView::renderTargetView;

    SceneView::SceneView(UINT width, UINT height)
    {
        D3D11_TEXTURE2D_DESC mDesc;

        mDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        mDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        mDesc.CPUAccessFlags = 0;
        mDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        mDesc.Width = 1280;
        mDesc.Height = 720;
        mDesc.ArraySize = 1;
        mDesc.SampleDesc.Count = 1;
        mDesc.SampleDesc.Quality = 0;
        mDesc.MipLevels = 1;
        mDesc.MiscFlags = 0;

        HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateTexture2D(&mDesc, nullptr, renderTargetTexture.GetAddressOf());

        D3D11_TEXTURE2D_DESC textureDesc;
        renderTargetTexture.Get()->GetDesc(&textureDesc);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = textureDesc.Format;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

        hr = GetDevice(Window::GetDxGraphic())->CreateShaderResourceView(renderTargetTexture.Get(), &srvDesc, renderTargetView.GetAddressOf());
    }

    void SceneView::Render()
    {
        ImGui::Begin("Game Window");

        // ��� ������ ���� ũ�� ��������
        ImVec2 availSize = ImGui::GetContentRegionAvail();

        // renderTargetTexture�� ���� ũ��(���⼭�� 1280x720�� �����Ǿ� ����)
        float textureWidth = 1280.0f;
        float textureHeight = 720.0f;
        float textureAspect = textureWidth / textureHeight;
        float availAspect = availSize.x / availSize.y;

        float drawWidth, drawHeight;

        // ������ ������ ���� ũ�� ���� (letterbox ���)
        if (availAspect > textureAspect)
        {
            drawHeight = availSize.y;
            drawWidth = drawHeight * textureAspect;
        }

        else
        {
            drawWidth = availSize.x;
            drawHeight = drawWidth / textureAspect;
        }

        ImGui::Image(
            (ImTextureID)(intptr_t)renderTargetView.Get(),
            ImVec2{ drawWidth, drawHeight },
            ImVec2{ 0, 0 },
            ImVec2{ 1, 1 }
        );

        ImGui::End();
    }

    void SceneView::SetRenderPipeline()
    {
        
    }
}