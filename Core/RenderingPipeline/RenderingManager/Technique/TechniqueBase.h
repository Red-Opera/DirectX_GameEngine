#pragma once
#include <limits>

// 전방 선언
namespace DynamicConstantBuffer { class Buffer; }

class Technique;
class RenderStep;

// 기법(Technique) 시스템을 방문하는 Visitor 패턴의 기본 클래스
class TechniqueBase
{
public:
    // 현재 방문 중인 기법 설정
    void SetTechnique(Technique* tech)
    {
        technique = tech;
        techIndex++;        // 방문한 기법 수 증가

        OnSetTechnique();   // 파생 클래스에서 오버라이드 가능한 콜백
    }

    // 현재 방문 중인 렌더 스텝 설정
    void SetRenderStep(RenderStep* step)
    {
        renderStep = step;
        renderStepIndex++; // 방문한 렌더 스텝 수 증가

        OnSetRenderStep(); // 파생 클래스에서 오버라이드 가능한 콜백
    }

    // 동적 상수 버퍼 방문 및 수정 여부 반환
    bool VisitBuffer(DynamicConstantBuffer::Buffer& buffer)
    {
        bufferIndex++;                  // 방문한 버퍼 수 증가

        return OnVisitBuffer(buffer);   // 실제 버퍼 처리는 파생 클래스에서 구현
    }

    virtual ~TechniqueBase() { }

protected:
    // 기법 설정 시 호출되는 가상 콜백 함수
    virtual void OnSetTechnique() { }
    
    // 렌더 스텝 설정 시 호출되는 가상 콜백 함수
    virtual void OnSetRenderStep() { }
    
    // 버퍼 방문 시 호출되는 가상 콜백 함수 (버퍼 수정 여부 반환)
    virtual bool OnVisitBuffer(DynamicConstantBuffer::Buffer&) { return false; }

    Technique* technique = nullptr;   // 현재 방문 중인 기법
    RenderStep* renderStep = nullptr; // 현재 방문 중인 렌더 스텝

#undef max
    // 방문 추적을 위한 인덱스들 (최대값으로 초기화하여 첫 방문 감지)
    size_t techIndex = std::numeric_limits<size_t>::max();          // 기법 방문 인덱스
    size_t renderStepIndex = std::numeric_limits<size_t>::max();    // 렌더 스텝 방문 인덱스
    size_t bufferIndex = std::numeric_limits<size_t>::max();        // 버퍼 방문 인덱스
};