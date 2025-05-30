#include "stdafx.h"
#include "ObjectPicker.h"

#include "Core/Component/MeshComponent.h"
#include "Core/Component/Transform/TransformComponent.h"
#include "Core/Draw/Base/Drawable.h"
#include "Core/Draw/Mesh.h"
#include "Core/Object/Object.h"
#include "Core/Scene/Base/SceneGraph.h"

#include <vector>

#undef max

void Engine::ObjectPicker::CreateRayFromScreenPoint
(
    int screenX, int screenY, 
    DirectX::XMVECTOR& rayOrigin, 
    DirectX::XMVECTOR& rayDirection, 
    const DirectX::XMMATRIX& viewMatrix, 
    const DirectX::XMMATRIX& projMatrix, 
    int screenWidth, int screenHeight
)
{
    // 화면 좌표를 NDC 좌표로 변환 (-1 ~ 1 범위)
    float ndcX = (2.0f * screenX / screenWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY / screenHeight);

    // 근평면과 원평면의 점 생성
    DirectX::XMVECTOR nearPoint = DirectX::XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
    DirectX::XMVECTOR farPoint = DirectX::XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    // 투영 행렬과 뷰 행렬의 곱의 역행렬 계산
    DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(viewMatrix, projMatrix);
    DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(nullptr, viewProj);

    // 월드 공간으로 변환
    DirectX::XMVECTOR nearWorldPoint = DirectX::XMVector4Transform(nearPoint, invViewProj);
    DirectX::XMVECTOR farWorldPoint = DirectX::XMVector4Transform(farPoint, invViewProj);

    // w 성분으로 나누어 정규화
    nearWorldPoint = DirectX::XMVectorDivide(nearWorldPoint, DirectX::XMVectorSplatW(nearWorldPoint));
    farWorldPoint = DirectX::XMVectorDivide(farWorldPoint, DirectX::XMVectorSplatW(farWorldPoint));

    // 레이 원점과 방향 설정
    rayOrigin = nearWorldPoint;
    rayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(farWorldPoint, nearWorldPoint));
}

std::shared_ptr<Object> Engine::ObjectPicker::PickObjectAtScreenPoint
(
    int screenX, int screenY, 
    const std::vector<std::shared_ptr<Object>>& objects, 
    const DirectX::XMMATRIX& viewMatrix, 
    const DirectX::XMMATRIX& projMatrix, 
    int screenWidth, int screenHeight
)
{
    using namespace Engine;

    DirectX::XMVECTOR rayOrigin, rayDirection;
    CreateRayFromScreenPoint(screenX, screenY, rayOrigin, rayDirection, viewMatrix, projMatrix, screenWidth, screenHeight);

    float closestDistance = FLT_MAX;
    std::shared_ptr<Object> closestObject = nullptr;

    // 모든 오브젝트에 대해 레이 교차 검사
    for (const auto& object : objects)
    {
        // 비활성화된 오브젝트는 건너뜀
        if (!object->GetActive())
            continue;

        // MeshComponent가 있는지 확인
        //auto meshComp = object->GetComponent<MeshComponent>();
        //if (!meshComp || !meshComp->GetEnable())
        //    continue;

        // 충돌 검사
        float distance = 0.0f;

        if (IntersectRayWithObject(rayOrigin, rayDirection, object, distance) && distance < closestDistance)
        {
            closestDistance = distance;
            closestObject = object;
        }
    }

    return closestObject;
}

bool Engine::ObjectPicker::IntersectRayWithObject
(
    const DirectX::XMVECTOR& rayOrigin, 
    const DirectX::XMVECTOR& rayDirection, 
    const std::shared_ptr<Object>& object, 
    float& outDistance
)
{
    auto transform = object->GetComponent<TransformComponent>();
    if (!transform)
        return false;

    // 오브젝트의 위치 (중심점)
    DirectX::XMVECTOR objectPos = Vector::ConvertXMVECTOR(transform->GetPosition());

    // 오브젝트의 크기 (충돌 반경 계산용)
    Scale scale = transform->GetScale();
    float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    float radius = maxScale * 0.5f; // 간단한 구체 충돌 검사 사용

    // 레이-구체 교차 검사
    DirectX::XMVECTOR oc = DirectX::XMVectorSubtract(rayOrigin, objectPos);
    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(rayDirection, rayDirection));
    float b = 2.0f * DirectX::XMVectorGetX(DirectX::XMVector3Dot(oc, rayDirection));
    float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(oc, oc)) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return false; // 교차 없음

    // 교차 거리 계산
    outDistance = (-b - sqrtf(discriminant)) / (2.0f * a);

    // 양수 거리만 유효
    return outDistance > 0;
}