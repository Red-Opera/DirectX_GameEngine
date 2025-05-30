#include "stdafx.h"
#include "CameraViewFrustumCulling.h"

#include <format>

using namespace DirectX;

CameraViewFrustumCulling::CameraViewFrustumCulling() : viewProjection()
{
    // 초기화
    for (int i = 0; i < Plane::Count; i++)
        frustumPlanes[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

void CameraViewFrustumCulling::UpdateFromMatrices(const XMMATRIX& view, const XMMATRIX& projection)
{
    // view-projection 행렬 계산
    XMMATRIX viewProj = XMMatrixMultiply(view, projection);

    UpdateFromViewProjection(viewProj);
}

void CameraViewFrustumCulling::UpdateFromViewProjection(const DirectX::XMMATRIX& viewProjection)
{
    // 행렬을 전치하여 계산을 쉽게 만듦
    XMMATRIX viewProjT = XMMatrixTranspose(viewProjection);

    // 각 절두체 평면 계산
    XMVECTOR rowX, rowY, rowZ, rowW;

    // XMMATRIX를 행별로 분해
    rowX = viewProjT.r[0];
    rowY = viewProjT.r[1];
    rowZ = viewProjT.r[2];
    rowW = viewProjT.r[3];

    // LEFT 평면 (행렬의 네 번째 행 + 첫 번째 행)
    XMStoreFloat4(&frustumPlanes[Plane::Left], XMPlaneNormalize(XMVectorAdd(rowW, rowX)));

    // RIGHT 평면 (행렬의 네 번째 행 - 첫 번째 행)
    XMStoreFloat4(&frustumPlanes[Plane::Right], XMPlaneNormalize(XMVectorSubtract(rowW, rowX)));

    // BOTTOM 평면 (행렬의 네 번째 행 + 두 번째 행)
    XMStoreFloat4(&frustumPlanes[Plane::Bottom], XMPlaneNormalize(XMVectorAdd(rowW, rowY)));

    // TOP 평면 (행렬의 네 번째 행 - 두 번째 행)
    XMStoreFloat4(&frustumPlanes[Plane::Top], XMPlaneNormalize(XMVectorSubtract(rowW, rowY)));

    // NEAR 평면 (행렬의 네 번째 행 + 세 번째 행)
    XMStoreFloat4(&frustumPlanes[Plane::Near], XMPlaneNormalize(XMVectorAdd(rowW, rowZ)));

    // FAR 평면 (행렬의 네 번째 행 - 세 번째 행)
    XMStoreFloat4(&frustumPlanes[Plane::Far], XMPlaneNormalize(XMVectorSubtract(rowW, rowZ)));
}

bool CameraViewFrustumCulling::CheckSphere(const XMFLOAT3& center, float radius) const
{
    // 모든 평면에 대해 구가 평면 바깥쪽에 있는지 확인
    for (int i = 0; i < Plane::Count; i++)
    {
        // 평면 방정식의 점 대입: Ax + By + Cz + D = 거리
        // SIMD 연산 대신 직접 스칼라 계산으로 최적화
        float distance = frustumPlanes[i].x * center.x +
            frustumPlanes[i].y * center.y +
            frustumPlanes[i].z * center.z +
            frustumPlanes[i].w;

        // 모든 평면에 대한 거리 출력
        //OutputDebugStringA(std::format("Center: ({:.3f}, {:.3f}, {:.3f}), Radius: {:.3f}, Distance to plane {}: {:.3f}\n",
            //center.x, center.y, center.z, radius, i, distance).c_str());

        // 구의 중심이 평면으로부터 반지름보다 멀리 있으면 절두체 외부에 있음
        if (distance < -radius)
            return false;
    }

    return true;
}

bool CameraViewFrustumCulling::CheckBox(const XMFLOAT3& center, const XMFLOAT3& extents) const
{
    // 모든 평면에 대해 바운딩 박스가 완전히 밖에 있는지 확인
    for (int i = 0; i < Plane::Count; i++)
    {
        // 평면의 법선 벡터
        XMVECTOR plane = XMLoadFloat4(&frustumPlanes[i]);
        float nx = frustumPlanes[i].x;
        float ny = frustumPlanes[i].y;
        float nz = frustumPlanes[i].z;

        // 바운딩 박스에서 평면 법선 방향으로 가장 먼 꼭지점 구하기
        float ex = extents.x * fabs(nx) + extents.y * fabs(ny) + extents.z * fabs(nz);

        // 중심에서 평면까지의 거리 계산
        XMVECTOR centerVec = XMLoadFloat3(&center);
        float distance = XMVectorGetX(XMPlaneDotCoord(plane, centerVec));

        // 가장 먼 꼭지점이 평면 바깥쪽에 있으면 바운딩 박스는 절두체 외부에 있음
        if (distance < -ex)
            return false;
    }

    return true;
}