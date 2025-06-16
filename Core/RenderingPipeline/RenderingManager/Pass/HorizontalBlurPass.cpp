#include "stdafx.h"
#include "HorizontalBlurPass.h"

#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/Exception/RenderGraphCompileException.h"

#include <sstream>

using namespace Graphic;

namespace RenderGraphNameSpace
{
    // 패스 이름과 해상도로 수평 블러 패스 생성
    HorizontalBlurPass::HorizontalBlurPass(std::string name, unsigned int width, unsigned int height)
        : PostProcessFullScreenRenderPass(std::move(name))
    {
        // 입력 해상도 유효성 검증
        if (width == 0 || height == 0)
        {
            std::ostringstream oss;
            oss << "수평 블러 패스 생성 오류: 잘못된 해상도가 입력되었습니다.\n"
                << "입력된 해상도: " << width << " x " << height << "\n"
                << "해상도는 0보다 큰 값이어야 합니다.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        // 해상도가 너무 작은 경우 경고
        if (width < 32 || height < 32)
        {
            std::ostringstream oss;
            oss << "수평 블러 패스 경고: 해상도가 매우 작습니다.\n"
                << "입력된 해상도: " << width << " x " << height << "\n"
                << "최소 권장 해상도: 32 x 32\n"
                << "블러 품질에 영향을 줄 수 있습니다.";

            // 경고이므로 예외를 발생시키지 않음
        }

        try
        {
            // 수평 블러 효과를 위한 렌더링 파이프라인 구성
            AddRender(PixelShader::GetRender("Shader/PostProcessing/OutlineBlur.hlsl"));    // 아웃라인 블러 셰이더
            AddRender(ColorBlend::GetRender(false));                                        // 컬러 블렌딩 비활성화
            AddRender(SamplerState::GetRender(SamplerState::TextureFilter::Point));         // 포인트 필터링 (픽셀 단위 정확성)

            // 입력 데이터 소비자들 등록
            AddRenderDataConsumer<RenderTarget>("scratchIn");                               // 입력 텍스처
            AddRenderDataConsumer<CachingPixelConstantBufferEx>("kernel");                  // 블러 커널 데이터
            AddDataConsumer(DirectRenderPipelineDataConsumer<CachingPixelConstantBufferEx>::Create("direction", direction)); // 방향 제어

            // 절반 해상도의 출력 렌더 타겟 생성 (성능 최적화)
            renderTarget = std::make_shared<ShaderInputRenderTarget>(width / 2, height / 2, 0u);
            AddDataProvider(DirectRenderPipelineDataProvider<RenderTarget>::Create("scratchOut", renderTarget));
        }
        catch (const std::exception& e)
        {
            std::ostringstream oss;
            oss << "수평 블러 패스 초기화 오류: 렌더링 파이프라인 설정에 실패했습니다.\n"
                << "패스 이름: [" << GetName() << "]\n"
                << "타겟 해상도: " << (width / 2) << " x " << (height / 2) << "\n"
                << "오류 내용: " << e.what() << "\n"
                << "셰이더 파일과 시스템 리소스를 확인해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }

    // 수평 블러 효과 실행
    void HorizontalBlurPass::Execute() NOEXCEPTRELEASE
    {
        // 방향 제어 상수 버퍼가 유효한지 확인
        if (direction == nullptr)
        {
            std::ostringstream oss;
            oss << "수평 블러 패스 실행 오류: 방향 제어 상수 버퍼가 초기화되지 않았습니다.\n"
                << "패스 이름: [" << GetName() << "]\n"
                << "데이터 소비자 연결을 확인해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        try
        {
            // 상수 버퍼에서 현재 데이터 가져오기
            auto buffer = direction->GetBuffer();
            
            // 수평 블러 방향 설정 (true = 수평, false = 수직)
            buffer["isHorizontal"] = true;

            // 수정된 데이터를 상수 버퍼에 설정
            direction->SetBuffer(buffer);
            direction->SetRenderPipeline();

            // 부모 클래스의 전체 화면 쿼드 렌더링 실행
            PostProcessFullScreenRenderPass::Execute();
        }

        catch (const std::exception& e)
        {
            std::ostringstream oss;
            oss << "수평 블러 패스 실행 오류: 블러 효과 적용에 실패했습니다.\n"
                << "패스 이름: [" << GetName() << "]\n"
                << "오류 내용: " << e.what() << "\n"
                << "상수 버퍼 데이터와 셰이더 연결을 확인해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }
}