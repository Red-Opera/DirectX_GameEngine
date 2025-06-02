#include "stdafx.h"
#include "TransformComponent.h"
#include "Core/Object/Object.h" // Object 클래스 포함 확인

TransformComponent::TransformComponent(std::shared_ptr<Object> object)
    : Component(object)
{
    // worldTransform과 localTransform은 Transform 기본 생성자에 의해
    // position = zero, rotation = identity quaternion, scale = one으로 초기화됩니다.
}

void TransformComponent::SetPosition(Position position) noexcept
{
    // 월드 위치 설정
    worldTransform.SetPosition(position);

    // 부모가 있는 경우 로컬 위치 계산 및 업데이트
    if (HasParent())
    {
        // 월드 위치를 부모의 로컬 좌표계로 변환
        XMVECTOR worldPosVec = Vector::ConvertXMVECTOR(position);
        XMMATRIX parentWorldToLocalMatrix = XMMatrixInverse(nullptr, parent->GetTransformMatrix());
        XMVECTOR localPosVec = XMVector3TransformCoord(worldPosVec, parentWorldToLocalMatrix);
        
        XMFLOAT3 localPosFloat3;
        XMStoreFloat3(&localPosFloat3, localPosVec);
        localTransform.SetPosition(localPosFloat3);
    }
    // 부모가 없는 경우 월드 위치 = 로컬 위치
    else
        localTransform.SetPosition(position);
}

void TransformComponent::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
    SetPosition(Vector3(position.x, position.y, position.z));
}

void TransformComponent::SetPosition(float x, float y, float z) noexcept
{
    SetPosition(Vector3(x, y, z));
}

Position& TransformComponent::GetPosition() noexcept
{
    return worldTransform.GetPosition();
}

// =============================================
// [World Rotation - 월드 회전 관리]
// =============================================

void TransformComponent::SetRotation(const Quaternion& rotation) noexcept
{
    worldTransform.SetRotation(rotation); // 내부에서 자동으로 정규화됨
    UpdateLocalRotation(); // 월드 회전 변경 시 로컬 회전 업데이트
}

void TransformComponent::SetRotationFromEuler(const Euler& eulerAngles) noexcept
{
    worldTransform.SetRotationFromEuler(eulerAngles);
    UpdateLocalRotation();
}

void TransformComponent::SetRotationFromEuler(float roll, float pitch, float yaw) noexcept
{
    worldTransform.SetRotationFromEuler(roll, pitch, yaw);
    UpdateLocalRotation();
}

Quaternion& TransformComponent::GetRotation() noexcept
{
    return worldTransform.GetRotation();
}

Vector3 TransformComponent::GetRotationEuler() const noexcept
{
    return worldTransform.GetRotationEuler();
}

void TransformComponent::SetScale(Scale scale) noexcept
{
    // 월드 스케일 설정
    worldTransform.SetScale(scale);

    // 부모가 있는 경우 로컬 스케일 계산
    if (HasParent())
    {
        const Scale& parentScale = parent->GetScale();

        // 0으로 나누기 방지
        if (parentScale.x != 0 && parentScale.y != 0 && parentScale.z != 0)
            localTransform.SetScale(scale.x / parentScale.x, scale.y / parentScale.y, scale.z / parentScale.z);

        // 부모 스케일이 0인 경우 월드 스케일과 동일하게 설정
        else
            localTransform.SetScale(scale); 
    }

    // 부모가 없는 경우 월드 스케일 = 로컬 스케일
    else
        localTransform.SetScale(scale);
}

void TransformComponent::SetScale(DirectX::XMFLOAT3 scale) noexcept
{
    SetScale(Scale(scale.x, scale.y, scale.z));
}

void TransformComponent::SetScale(float x, float y, float z) noexcept
{
    SetScale(Scale(x, y, z));
}

Scale& TransformComponent::GetScale() noexcept
{
    return worldTransform.GetScale();
}

// =============================================
// [Direction Vectors - 방향 벡터 계산]
// =============================================

const Vector3 TransformComponent::GetRight() const noexcept
{
    // 월드 회전 쿼터니언을 회전 행렬로 변환
    const Quaternion& quat = worldTransform.GetRotation();
    XMVECTOR quatVec = XMVectorSet(quat.x, quat.y, quat.z, quat.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);
    
    // 기본 오른쪽 벡터 (1, 0, 0)를 회전 적용
    XMVECTOR rightVector = XMLoadFloat3(&Vector::right);
    rightVector = XMVector3TransformNormal(rightVector, rotationMatrix);
    rightVector = XMVector3Normalize(rightVector);

    XMFLOAT3 right;
    XMStoreFloat3(&right, rightVector);

    return Vector3(right.x, right.y, right.z);
}

const Vector3 TransformComponent::GetUp() const noexcept
{
    // 월드 회전 쿼터니언을 회전 행렬로 변환
    const Quaternion& quat = worldTransform.GetRotation();
    XMVECTOR quatVec = XMVectorSet(quat.x, quat.y, quat.z, quat.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);

    // 기본 위쪽 벡터 (0, 1, 0)를 회전 적용
    XMVECTOR upVector = XMLoadFloat3(&Vector::up);
    upVector = XMVector3TransformNormal(upVector, rotationMatrix);
    upVector = XMVector3Normalize(upVector);

    XMFLOAT3 up;
    XMStoreFloat3(&up, upVector);

    return Vector3(up.x, up.y, up.z);
}

const Vector3 TransformComponent::GetForward() const noexcept
{
    // 월드 회전 쿼터니언을 회전 행렬로 변환
    const Quaternion& quat = worldTransform.GetRotation();
    XMVECTOR quatVec = XMVectorSet(quat.x, quat.y, quat.z, quat.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);

    // 기본 앞쪽 벡터 (0, 0, 1)를 회전 적용
    XMVECTOR forwardVector = XMLoadFloat3(&Vector::forward);
    forwardVector = XMVector3TransformNormal(forwardVector, rotationMatrix);
    forwardVector = XMVector3Normalize(forwardVector);

    XMFLOAT3 forward;
    XMStoreFloat3(&forward, forwardVector);

    return Vector3(forward.x, forward.y, forward.z);
}

Transform& TransformComponent::GetTransform() noexcept
{
    return worldTransform;
}

// =============================================
// [Local Rotation - 로컬 회전 관리]
// =============================================

void TransformComponent::SetLocalRotation(const Quaternion& rotation) noexcept
{
    localTransform.SetRotation(rotation); // 내부에서 자동으로 정규화됨
    UpdateWorldRotation(); // 로컬 회전 변경 시 월드 회전 업데이트
}

void TransformComponent::SetLocalRotationFromEuler(const Euler& eulerAngles) noexcept
{
    localTransform.SetRotationFromEuler(eulerAngles);
    UpdateWorldRotation();
}

void TransformComponent::SetLocalRotationFromEuler(float roll, float pitch, float yaw) noexcept
{
    localTransform.SetRotationFromEuler(roll, pitch, yaw);
    UpdateWorldRotation();
}

Quaternion& TransformComponent::GetLocalRotation() noexcept
{
    return localTransform.GetRotation();
}

Vector3 TransformComponent::GetLocalRotationEuler() const noexcept
{
    return localTransform.GetRotationEuler();
}

// =============================================
// [Local Position & Scale - 로컬 위치 및 크기]
// =============================================

void TransformComponent::SetLocalPosition(Position position) noexcept
{
    // 로컬 위치 설정
    localTransform.SetPosition(position);

    // 부모가 있는 경우 월드 위치 계산
    if (HasParent())
    {
        // 로컬 위치를 부모의 월드 좌표계로 변환
        XMVECTOR localPosVec = Vector::ConvertXMVECTOR(position);
        XMMATRIX parentWorldMatrix = parent->GetTransformMatrix();
        XMVECTOR worldPosVec = XMVector3TransformCoord(localPosVec, parentWorldMatrix);
        
        XMFLOAT3 worldPosFloat3;
        XMStoreFloat3(&worldPosFloat3, worldPosVec);
        worldTransform.SetPosition(worldPosFloat3);
    }

    // 부모가 없는 경우 로컬 위치 = 월드 위치
    else
        worldTransform.SetPosition(position);
}

void TransformComponent::SetLocalPosition(float x, float y, float z) noexcept
{
    SetLocalPosition(Position(x, y, z));
}

void TransformComponent::SetLocalScale(Scale scale) noexcept
{
    // 로컬 스케일 설정
    localTransform.SetScale(scale);

    // 부모가 있는 경우 월드 스케일 계산
    if (HasParent())
    {
        const Scale& parentScale = parent->GetScale();
        // 월드 스케일 = 로컬 스케일 * 부모 월드 스케일
        worldTransform.SetScale(scale.x * parentScale.x, scale.y * parentScale.y, scale.z * parentScale.z);
    }

    // 부모가 없는 경우 로컬 스케일 = 월드 스케일
    else
        worldTransform.SetScale(scale);
}

void TransformComponent::SetLocalScale(DirectX::XMFLOAT3 scale) noexcept
{
    SetLocalScale(Scale(scale.x, scale.y, scale.z));
}

void TransformComponent::SetLocalScale(float x, float y, float z) noexcept
{
    SetLocalScale(Scale(x, y, z));
}

Transform& TransformComponent::GetLocalTransform() noexcept
{
    return localTransform;
}

Position& TransformComponent::GetLocalPosition() noexcept
{
    return localTransform.GetPosition();
}

Scale& TransformComponent::GetLocalScale() noexcept
{
    return localTransform.GetScale();
}

// =============================================
// [Rotation Update Helpers - 회전 업데이트 헬퍼 함수]
// =============================================

void TransformComponent::UpdateWorldRotation() noexcept
{
    if (HasParent())
    {
        // 부모가 있는 경우: 월드 회전 = 부모 월드 회전 * 로컬 회전
        const Quaternion& localRot = localTransform.GetRotation();
        const Quaternion& parentRot = parent->GetRotation();

        XMVECTOR localRotQ = XMVectorSet(localRot.x, localRot.y, localRot.z, localRot.w);
        XMVECTOR parentRotQ = XMVectorSet(parentRot.x, parentRot.y, parentRot.z, parentRot.w);

        // 쿼터니언 곱셈으로 회전 결합 (순서 중요: Parent * Local)
        XMVECTOR worldRotQ = XMQuaternionMultiply(parentRotQ, localRotQ);
        worldRotQ = XMQuaternionNormalize(worldRotQ); // 정규화로 수치 오차 방지

        // 결과를 월드 회전에 저장
        Quaternion& worldQuat = worldTransform.GetRotation();
        worldQuat.x = XMVectorGetX(worldRotQ);
        worldQuat.y = XMVectorGetY(worldRotQ);
        worldQuat.z = XMVectorGetZ(worldRotQ);
        worldQuat.w = XMVectorGetW(worldRotQ);
    }

    // 부모가 없는 경우: 로컬 회전 = 월드 회전
    else
        worldTransform.SetRotation(localTransform.GetRotation());
}

void TransformComponent::UpdateLocalRotation() noexcept
{
    if (HasParent())
    {
        // 부모가 있는 경우: 로컬 회전 = 부모 월드 회전의 역 * 월드 회전
        const Quaternion& worldRot = worldTransform.GetRotation();
        const Quaternion& parentRot = parent->GetRotation();
        
        XMVECTOR worldRotQ = XMVectorSet(worldRot.x, worldRot.y, worldRot.z, worldRot.w);
        XMVECTOR parentRotQ = XMVectorSet(parentRot.x, parentRot.y, parentRot.z, parentRot.w);
        XMVECTOR parentRotInvQ = XMQuaternionInverse(parentRotQ); // 부모 회전의 역원
        
        // 로컬 회전 계산 (부모 회전 제거)
        XMVECTOR localRotQ = XMQuaternionMultiply(parentRotInvQ, worldRotQ);
        localRotQ = XMQuaternionNormalize(localRotQ); // 정규화로 수치 오차 방지

        // 결과를 로컬 회전에 저장
        Quaternion& localQuat = localTransform.GetRotation();
        localQuat.x = XMVectorGetX(localRotQ);
        localQuat.y = XMVectorGetY(localRotQ);
        localQuat.z = XMVectorGetZ(localRotQ);
        localQuat.w = XMVectorGetW(localRotQ);
    }

    // 부모가 없는 경우: 월드 회전 = 로컬 회전
    else
        localTransform.SetRotation(worldTransform.GetRotation());
}

// =============================================
// [Transform Matrix - 변환 행렬 생성]
// =============================================

DirectX::XMMATRIX TransformComponent::GetTransformMatrix() const noexcept
{
    // 부모가 있는 경우: 로컬 변환 행렬 * 부모의 월드 변환 행렬
    if (HasParent())
    {
        XMMATRIX localMatrix = GetLocalTransformMatrix();
        XMMATRIX parentMatrix = parent->GetTransformMatrix();
        return localMatrix * parentMatrix; // 행렬 곱셈으로 변환 결합
    }

    // 부모가 없는 경우: 월드 변환 행렬 직접 생성
    else
    {
        const Quaternion& rot = worldTransform.GetRotation();
        XMVECTOR rotQuat = XMVectorSet(rot.x, rot.y, rot.z, rot.w);
        
        // SRT 순서로 변환 행렬 생성: Scale * Rotation * Translation
        return
            XMMatrixScaling(worldTransform.GetScale().x, worldTransform.GetScale().y, worldTransform.GetScale().z) *
            XMMatrixRotationQuaternion(rotQuat) *
            XMMatrixTranslation(worldTransform.GetPosition().x, worldTransform.GetPosition().y, worldTransform.GetPosition().z);
    }
}

DirectX::XMFLOAT4X4& TransformComponent::GetTransformMatrix4x4() noexcept
{
    // 변환 행렬을 캐시에 저장하고 반환 (성능 최적화)
    XMStoreFloat4x4(&transformMatrix, GetTransformMatrix());

    return transformMatrix;
}

DirectX::XMMATRIX TransformComponent::GetLocalTransformMatrix() const noexcept
{
    // 로컬 변환 행렬 생성: Scale * Rotation * Translation
    const Quaternion& rot = localTransform.GetRotation();
    XMVECTOR rotQuat = XMVectorSet(rot.x, rot.y, rot.z, rot.w);
    
    return
        XMMatrixScaling(localTransform.GetScale().x, localTransform.GetScale().y, localTransform.GetScale().z) *
        XMMatrixRotationQuaternion(rotQuat) *
        XMMatrixTranslation(localTransform.GetPosition().x, localTransform.GetPosition().y, localTransform.GetPosition().z);
}

DirectX::XMFLOAT4X4& TransformComponent::GetLocalTransformMatrix4x4() noexcept
{
    // 로컬 변환 행렬을 캐시에 저장하고 반환
    XMStoreFloat4x4(&transformMatrix, GetLocalTransformMatrix());

    return transformMatrix;
}

// =============================================
// [Hierarchy Management - 계층 구조 관리]
// =============================================

void TransformComponent::SetParent(std::shared_ptr<TransformComponent> newParent) noexcept
{
    // 기존 부모로부터 자신을 제거
    if (parent)
        parent->RemoveChild(shared_from_this());

    parent = newParent;

    if (parent)
    {
        // 새 부모에 자신을 자식으로 추가
        parent->AddChild(shared_from_this());
        
        // 부모가 설정되면 현재 월드 변환을 유지하면서 로컬 변환을 재계산
        SetPosition(worldTransform.GetPosition()); // 위치에 대한 로컬 업데이트
        SetRotation(worldTransform.GetRotation()); // 회전에 대한 로컬 업데이트
        SetScale(worldTransform.GetScale());       // 스케일에 대한 로컬 업데이트
    }

    // 부모가 제거되면 로컬 변환이 곧 월드 변환이 됨
    else
        localTransform = worldTransform;
}

void TransformComponent::SetParent(std::shared_ptr<Object> parentObject) noexcept
{
    if (parentObject)
        SetParent(parentObject->GetComponent<TransformComponent>());
}

bool TransformComponent::HasParent() const noexcept
{
    return parent != nullptr;
}

void TransformComponent::RemoveParent() noexcept
{
    if (parent == nullptr)
        return;

    // 부모로부터 제거되기 전에 현재 월드 변환을 로컬 변환으로 설정
    // (부모가 사라지면 현재 월드 상태가 새로운 로컬 상태가 됨)
    worldTransform.SetPosition(GetPosition()); // 현재 월드 위치 가져오기
    worldTransform.SetRotation(GetRotation()); // 현재 월드 회전 가져오기
    worldTransform.SetScale(GetScale());       // 현재 월드 스케일 가져오기
    localTransform = worldTransform;           // 로컬을 월드로 설정

    parent->RemoveChild(this->shared_from_this());
    parent = nullptr;
}

void TransformComponent::AddChild(std::shared_ptr<TransformComponent> child) noexcept
{
    if (!child) 
        return;

    // 이미 자식으로 있는지 중복 확인
    for (const auto& c : children)
    {
        if (c == child) 
            return;
    }

    children.push_back(child);
    // 필요시 childIndex 맵도 업데이트 (현재는 Object 이름 기반)
}

void TransformComponent::AddChild(std::shared_ptr<Object> childObject) noexcept
{
    if (childObject)
        AddChild(childObject->GetComponent<TransformComponent>());
}

void TransformComponent::RemoveChild(std::shared_ptr<TransformComponent> child) noexcept
{
    if (!child) 
        return;

    // 자식 목록에서 제거
    auto it = std::remove(children.begin(), children.end(), child);

    if (it != children.end())
        children.erase(it, children.end());
}

void TransformComponent::RemoveChild(UINT index) noexcept
{
    if (index >= children.size())
        return;

    // 인덱스로 자식 제거
    children.erase(children.begin() + index);
}

void TransformComponent::RemoveChild(std::shared_ptr<Object> childObject) noexcept
{
    if (childObject)
        RemoveChild(childObject->GetComponent<TransformComponent>());
}

void TransformComponent::RemoveChild(std::string childObjectName) noexcept
{
    // 이름 기반 자식 제거 (childIndex 맵 사용)
    auto iter = childIndex.find(childObjectName);

    if (iter == childIndex.end()) 
        return;

    UINT indexToRemove = iter->second;

    children.erase(children.begin() + indexToRemove);
    childIndex.erase(iter);

    // 제거된 인덱스 이후의 모든 인덱스 업데이트
    for (size_t i = indexToRemove; i < children.size(); i++)
        childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

// =============================================
// [Child Query Functions - 자식 조회 함수]
// =============================================

bool TransformComponent::HasChild(std::shared_ptr<TransformComponent> child) const noexcept
{
    if (!child) 
        return false;

    // 자식 목록에서 검색
    for (const auto& c : children)
    {
        if (c == child) 
            return true;
    }

    return false;
}

bool TransformComponent::HasChild(std::shared_ptr<Object> childObject) const noexcept
{
    if (childObject)
        return HasChild(childObject->GetComponent<TransformComponent>());

    return false;
}

bool TransformComponent::HasChild(std::string childObjectName) const noexcept
{
    return childIndex.count(childObjectName) > 0;
}

bool TransformComponent::HasChild(UINT index) const noexcept
{
    return index < children.size();
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(std::shared_ptr<Object> childObject) noexcept
{
    if (childObject)
    {
        auto comp = childObject->GetComponent<TransformComponent>();

        if (HasChild(comp)) 
            return comp;
    }

    return nullptr;
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(std::string childObjectName) noexcept
{
    auto iter = childIndex.find(childObjectName);

    if (iter != childIndex.end())
        return children[iter->second];

    return nullptr;
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(UINT index) noexcept
{
    if (index < children.size())
        return children[index];

    return nullptr;
}

std::vector<std::shared_ptr<TransformComponent>> TransformComponent::GetChildrens() noexcept
{
    return children;
}

size_t TransformComponent::GetChildCount() const noexcept
{
    return children.size();
}

// =============================================
// [Transform Update - 변환 업데이트]
// =============================================

void TransformComponent::UpdateTransform() noexcept
{
    // 부모가 있다면, 부모의 변환에 따라 자신의 월드 변환을 업데이트
    if (HasParent())
    {
        // 위치 업데이트: 로컬 위치를 부모의 월드 변환으로 변환
        XMVECTOR localPosVec = Vector::ConvertXMVECTOR(localTransform.GetPosition());
        XMMATRIX parentWorldMatrix = parent->GetTransformMatrix();
        XMVECTOR worldPosVec = XMVector3TransformCoord(localPosVec, parentWorldMatrix);

        Position& worldPos = worldTransform.GetPosition();
        worldPos.x = XMVectorGetX(worldPosVec);
        worldPos.y = XMVectorGetY(worldPosVec);
        worldPos.z = XMVectorGetZ(worldPosVec);

        // 회전 업데이트: 쿼터니언 곱셈으로 부모 회전과 로컬 회전 결합
        UpdateWorldRotation();

        // 스케일 업데이트: 부모 스케일과 로컬 스케일 곱셈
        const Scale& parentScale = parent->GetScale();
        const Scale& localScale = localTransform.GetScale();
        worldTransform.SetScale(
            localScale.x * parentScale.x, 
            localScale.y * parentScale.y, 
            localScale.z * parentScale.z
        );
    }

    // 부모가 없다면, 로컬 변환이 곧 월드 변환
    else
        worldTransform = localTransform;

    // 자식들의 변환도 재귀적으로 업데이트
    for (const auto& child : children)
    {
        if (child != nullptr)
            child->UpdateTransform();
    }
}
