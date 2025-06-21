#pragma once

#include "Base/RenderQueuePass.h"

#include "Core/RenderingPipeline/RenderingManager/Pass/Base/RenderJob.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"

// 전방 선언
class DxGraphic;

namespace RenderGraphNameSpace
{
    // 카메라의 와이어프레임을 렌더링하는 패스 클래스
    class CameraWireFramePass : public RenderQueuePass
    {
    public:
        // 패스 이름으로 생성자 초기화 (렌더 타겟과 깊이 스텐실 설정)
        CameraWireFramePass(std::string name) : RenderQueuePass(std::move(name))
        {
            using namespace Graphic;

            AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));  // 렌더 타겟을 입력으로 받는 데이터 소비자 등록
            AddDataConsumer(DirectBufferDataConsumer<DepthStencil>::Create("depthStencil", depthStencil));  // 깊이 스텐실을 입력으로 받는 데이터 소비자 등록

            // 렌더 타겟을 출력으로 제공 (후처리 패스 등에서 렌더 객체로 활용될 수 있도록 Render 타입 제공자 사용)
            AddDataProvider(DirectRenderPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
            AddDataProvider(DirectBufferPipelineDataProvider<DepthStencil>::Create("depthStencil", depthStencil));  // 깊이 스텐실을 출력으로 제공하는 데이터 제공자 등록

            // 역순 깊이 테스트를 사용하는 스텐실 설정 (와이어프레임이 기존 객체 뒤에 렌더링되도록)
            AddRender(Stencil::GetRender(Stencil::DrawMode::DepthReversed));
        }
    };
}