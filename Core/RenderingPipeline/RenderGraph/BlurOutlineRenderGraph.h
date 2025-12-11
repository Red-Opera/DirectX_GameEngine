#pragma once

#include "RenderGraph.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"

#include <memory>

// 전방 선언
class DxGraphic;
class Camera;

namespace Graphic
{
    class Render;
    class RenderTarget;
    class ShadowSamplerState;
    class ShadowRasterizer;
}

namespace RenderGraphNameSpace
{
    // 블러 아웃라인 효과가 적용된 렌더 그래프를 구성하는 클래스
    class BlurOutlineRenderGraph : public RenderGraph
    {
    public:
        // 그래프 및 패스 구성, 커널 초기화
        BlurOutlineRenderGraph();

        // ImGui 기반 커널/섀도우 설정 창 렌더링
        void RenderWindows();

        // 메인 카메라/섀도우 카메라 전달
        void RenderMainCamera(Camera& camera);
        void RenderShadowCamera(Camera& camera);

        // 섀도우맵 덤프
        void DumpShadowMap(const std::string& path);

    private:
        // 블러 커널 타입 (가우시안/박스)
        enum class KernelType { Gauss, Box } kernelType = KernelType::Gauss;

        // 가우시안/박스 커널 설정
        void SetKernelGauss(int radius, float sigma) NOEXCEPTRELEASE;
        void SetKernelBox(int radius) NOEXCEPTRELEASE;

        // ImGui 커널/섀도우 설정 창
        void RenderKernelWindow();
        void RenderShadowWindow();

        // 블러 커널/방향 상수 버퍼
        std::shared_ptr<Graphic::CachingPixelConstantBufferEx> blurKernel;
        std::shared_ptr<Graphic::CachingPixelConstantBufferEx> blurDirection;

        static constexpr int maxRadius = 7; // 최대 커널 반경
        int radius = 4;                     // 현재 커널 반경
        float sigma = 2.0f;                 // 가우시안 시그마
    };
}