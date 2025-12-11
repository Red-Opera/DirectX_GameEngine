#pragma once

#include "../Pass/Base/RenderPass.h"

namespace Graphic { class BufferResource; }

namespace RenderGraphNameSpace
{
    // 버퍼 리소스를 클리어하는 렌더 패스 클래스
    class BufferPassClear : public RenderPass
    {
    public:
        // 패스 이름으로 생성자 초기화 (입력과 출력을 동시에 등록)
        BufferPassClear(std::string name);

        // 버퍼 클리어 작업을 수행하는 실행 메서드
        void Execute() NOEXCEPTRELEASE override;

    private:
        std::shared_ptr<Graphic::BufferResource> buffer; // 클리어할 버퍼 리소스
    };
}