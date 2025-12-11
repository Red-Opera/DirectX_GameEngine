#pragma once

#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/VertexShader.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/Rasterizer.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderQueuePass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/Exception/RenderGraphCompileException.h"

#include <vector>
#include <sstream>

class DxGraphic;

namespace RenderGraphNameSpace
{
    // 블러 효과가 적용된 아웃라인을 렌더링하는 패스 클래스
    class BlurOutlineRenderingPass : public RenderQueuePass
    {
    public:
        // 패스 이름과 해상도로 블러 아웃라인 패스 생성
        BlurOutlineRenderingPass(std::string name, unsigned int width, unsigned int height)
            : RenderQueuePass(std::move(name))
        {
            // 입력 해상도 유효성 검증
            if (width == 0 || height == 0)
            {
                std::ostringstream oss;
                oss << "블러 아웃라인 패스 생성 오류: 잘못된 해상도가 입력되었습니다.\n"
                    << "입력된 해상도: " << width << " x " << height << "\n"
                    << "해상도는 0보다 큰 값이어야 합니다.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 해상도가 너무 작은 경우 경고
            if (width < 32 || height < 32)
            {
                std::ostringstream oss;
                oss << "블러 아웃라인 패스 경고: 해상도가 매우 작습니다.\n"
                    << "입력된 해상도: " << width << " x " << height << "\n"
                    << "최소 권장 해상도: 32 x 32\n"
                    << "성능과 품질에 영향을 줄 수 있습니다.";
                // 경고이므로 예외를 발생시키지 않고 로그 출력 등을 고려
            }

            using namespace Graphic;

            // 절반 해상도의 렌더 타겟 생성 (블러 효과를 위한 성능 최적화)
            try
            {
                renderTarget = std::make_unique<ShaderInputRenderTarget>(width / 2, height / 2, 0);
            }

            catch (const std::exception& e)
            {
                std::ostringstream oss;
                oss << "블러 아웃라인 패스 생성 오류: 렌더 타겟 생성에 실패했습니다.\n"
                    << "타겟 해상도: " << (width / 2) << " x " << (height / 2) << "\n"
                    << "오류 내용: " << e.what() << "\n"
                    << "시스템 메모리와 GPU 메모리를 확인해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 블러 아웃라인 렌더링 파이프라인 구성
            AddRender(VertexShader::GetRender("Shader/ColorShader.hlsl"));   // 컬러 버텍스 셰이더
            AddRender(PixelShader::GetRender("Shader/ColorShader.hlsl"));    // 컬러 픽셀 셰이더
            AddRender(Stencil::GetRender(Stencil::DrawMode::Mask));          // 스텐실 마스킹 (아웃라인 영역 제한)
            AddRender(ColorBlend::GetRender(false));                         // 컬러 블렌딩 비활성화

            // 생성된 렌더 타겟을 "scratchOut" 이름으로 출력 제공
            AddDataProvider(DirectRenderPipelineDataProvider<Graphic::RenderTarget>::Create("scratchOut", renderTarget));
        }

        // 블러 아웃라인 렌더링 실행
        void Execute() NOEXCEPTRELEASE override
        {
            // 렌더 타겟이 유효한지 확인
            if (renderTarget == nullptr)
            {
                std::ostringstream oss;
                oss << "블러 아웃라인 패스 실행 오류: 렌더 타겟이 초기화되지 않았습니다.\n"
                    << "패스 이름: [" << GetName() << "]\n"
                    << "생성자에서 렌더 타겟 초기화를 확인해주세요.";

                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 1. 렌더 타겟을 클리어하여 깨끗한 상태로 초기화
            renderTarget->Clear();

            // 2. 부모 클래스의 실행 로직 호출 (큐에 있는 모든 렌더 작업 수행)
            RenderQueuePass::Execute();
        }
    };
}