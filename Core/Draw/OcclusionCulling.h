#pragma once
#include <wrl/client.h>
#include <d3d11.h>

class OcclusionCulling
{
public:
    OcclusionCulling();
    ~OcclusionCulling() = default;

    // Occlusion Query를 시작합니다.
    void BeginQuery();

    // Occlusion Query를 종료합니다.
    void EndQuery();

    // query 결과를 조회하여 오브젝트가 보이는지 판정합니다.
    bool IsVisible() const;

    // 쿼리 결과를 업데이트하고 이전 프레임의 결과 반환
    bool UpdateVisibility();

private:
    enum class QueryState { Ready, Started, Ended };

    mutable QueryState m_queryState = QueryState::Ready;

    Microsoft::WRL::ComPtr<ID3D11Query> occlusionQuery;

    bool m_lastFrameVisible = true; // 초기값은 보이는 것으로 설정
    bool m_resultReady = false;
};
