#pragma once

#include "Core/Exception/GraphicsException.h"

class DxGraphic;

namespace Graphic
{
    // 렌더링 버퍼 리소스의 추상 기본 클래스
    class BufferResource
    {
    public:
        virtual void RenderAsBuffer() NOEXCEPTRELEASE = 0;                  // 단독으로 버퍼를 렌더링 타겟으로 설정하는 순수 가상 함수
        virtual void RenderAsBuffer(BufferResource*) NOEXCEPTRELEASE = 0;   // 다른 버퍼와 함께 멀티 렌더 타겟으로 설정하는 순수 가상 함수
        virtual void Clear() NOEXCEPTRELEASE = 0;                           // 버퍼 내용을 지우는 순수 가상 함수

        virtual ~BufferResource() = default;
    };
}