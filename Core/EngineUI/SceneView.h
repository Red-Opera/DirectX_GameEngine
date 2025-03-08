#pragma once

#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx11.h"
#include "External/Imgui/imgui_impl_win32.h"

#include "Core/RenderingPipeline/Render.h"

#include <d3d11.h>
#include <wrl.h>

class DxGraphic;

namespace Graphic
{
    class SceneView : public Render
    {
    public:
        SceneView(UINT width, UINT height);

        void Render();

        // Render을(를) 통해 상속됨
        void SetRenderPipeline() override;

    private:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTexture = nullptr;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTargetView = nullptr;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget = nullptr;
    };
}