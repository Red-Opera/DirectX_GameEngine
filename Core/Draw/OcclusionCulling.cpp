#include "stdafx.h"
#include "OcclusionCulling.h"

#include "Core/Window.h"
#include "Core/DxGraphic.h"
#include "d3d11.h" // 추가된 헤더 파일

#include <cassert>
#include <thread>

OcclusionCulling::OcclusionCulling()
{
    D3D11_QUERY_DESC queryDesc = {};
    queryDesc.Query = D3D11_QUERY_OCCLUSION;
    queryDesc.MiscFlags = 0;

    HRESULT hr = Window::GetDxGraphic().GetDevice()->CreateQuery(&queryDesc, occlusionQuery.ReleaseAndGetAddressOf());

    assert(SUCCEEDED(hr));
}

void OcclusionCulling::BeginQuery()
{
    Window::GetDxGraphic().GetDeviceContext()->Begin(occlusionQuery.Get());
}

void OcclusionCulling::EndQuery()
{
    Window::GetDxGraphic().GetDeviceContext()->End(occlusionQuery.Get());
}

bool OcclusionCulling::IsVisible() const
{
    return m_lastFrameVisible;
}

bool OcclusionCulling::UpdateVisibility()
{
    UINT64 sampleCount = 0;

    // 논블로킹 방식으로 쿼리 결과 받기 (기다리지 않음)
    HRESULT hr = Window::GetDxGraphic().GetDeviceContext()->GetData(
        occlusionQuery.Get(),
        &sampleCount,
        sizeof(sampleCount),
        D3D11_ASYNC_GETDATA_DONOTFLUSH);

    if (hr == S_OK)
    {
        // 결과를 받았을 경우에만 상태 업데이트
        m_lastFrameVisible = (sampleCount > 0);
        m_resultReady = true;
    }

    else if (hr == S_FALSE)
    {
        // 결과가 아직 준비되지 않음 - 이전 프레임 값 사용
        // 로그 출력 제거 (성능 저하 원인)
    }

    return m_lastFrameVisible;
}
