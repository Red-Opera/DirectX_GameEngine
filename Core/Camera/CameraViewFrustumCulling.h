#pragma once
#include <DirectXMath.h>

class CameraViewFrustumCulling
{
public:
    CameraViewFrustumCulling();

    // 카메라의 행렬에서 Frustum 정보 업데이트
    void UpdateFromMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	void UpdateFromViewProjection(const DirectX::XMMATRIX& viewProjection);

    // 바운딩 박스가 절두체 내에 있는지 확인
    bool CheckBox(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents) const;

    // 바운딩 구가 절두체 내에 있는지 확인
    bool CheckSphere(const DirectX::XMFLOAT3& center, float radius) const;
    
	DirectX::XMMATRIX GetViewProjection() const { return viewProjection; }

private:
    enum Plane { Near, Far, Left, Right, Top, Bottom, Count };

    DirectX::XMFLOAT4 frustumPlanes[Plane::Count];  // 절두체 평면 방정식
	DirectX::XMMATRIX viewProjection;               // 뷰 - 프로젝션 행렬
};
