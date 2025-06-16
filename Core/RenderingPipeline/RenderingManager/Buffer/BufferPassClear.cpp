#include "stdafx.h"
#include "BufferPassClear.h"

#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"

namespace RenderGraphNameSpace
{
    // 패스 이름으로 생성자 초기화 (입력과 출력을 동시에 등록)
    BufferPassClear::BufferPassClear(std::string name)
        : RenderPass(std::move(name))
    {
        // "buffer"라는 이름으로 버퍼 리소스를 입력받는 데이터 소비자 등록
        AddDataConsumer(DirectBufferDataConsumer<Graphic::BufferResource>::Create("buffer", buffer));
        
        // "buffer"라는 이름으로 클리어된 버퍼를 출력하는 데이터 제공자 등록
        AddDataProvider(DirectBufferPipelineDataProvider<Graphic::BufferResource>::Create("buffer", buffer));
    }

    // 버퍼 클리어 작업을 수행하는 실행 메서드
    void BufferPassClear::Execute() NOEXCEPTRELEASE
    {
        // 연결된 버퍼 리소스의 Clear() 메서드 호출
        buffer->Clear();
    }
}