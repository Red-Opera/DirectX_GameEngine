#include "stdafx.h"
#include "TransformComponent.h"
#include "Core/Object/Object.h" // Object 클래스 포함 확인

TransformComponent::TransformComponent(std::shared_ptr<Object> object)
    : Component(object)
{
    // worldTransform과 localTransform은 Transform 기본 생성자에 의해
    // position = zero, rotation = identity, scale = one으로 초기화됩니다.
}

void TransformComponent::SetPosition(Position position) noexcept
{
    worldTransform.SetPosition(position);

    // 로컬 위치 업데이트 로직 (기존과 유사하게 유지, 회전과 무관)
    if (HasParent())
    {
        XMVECTOR worldPosVec = Vector::ConvertXMVECTOR(position);
        XMMATRIX parentWorldToLocalMatrix = XMMatrixInverse(nullptr, parent->GetTransformMatrix());
        XMVECTOR localPosVec = XMVector3TransformCoord(worldPosVec, parentWorldToLocalMatrix);
        XMFLOAT3 localPosFloat3;
        XMStoreFloat3(&localPosFloat3, localPosVec);
        localTransform.SetPosition(localPosFloat3);
    }

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

// World Rotation 설정
void TransformComponent::SetRotation(const Quaternion& rotation) noexcept
{
    worldTransform.SetRotation(rotation); // 내부에서 정규화
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
    worldTransform.SetScale(scale);

    // 로컬 스케일 업데이트 (기존 로직 유지)
    if (HasParent())
    {
        const Scale& parentScale = parent->GetScale();

        if (parentScale.x != 0 && parentScale.y != 0 && parentScale.z != 0) // 0으로 나누기 방지
            localTransform.SetScale(scale.x / parentScale.x, scale.y / parentScale.y, scale.z / parentScale.z);

        // 부모 스케일이 0이면 월드 스케일과 동일하게 (또는 오류 처리)
        else
            localTransform.SetScale(scale); 
    }

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

const Vector3 TransformComponent::GetRight() const noexcept
{
    // worldTransform.rotation (Quaternion)을 사용
    const Quaternion& quat = worldTransform.GetRotation();
    XMVECTOR quatVec = XMVectorSet(quat.x, quat.y, quat.z, quat.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);
    XMVECTOR rightVector = XMLoadFloat3(&Vector::right);

    rightVector = XMVector3TransformNormal(rightVector, rotationMatrix);
    rightVector = XMVector3Normalize(rightVector);

    XMFLOAT3 right;
    XMStoreFloat3(&right, rightVector);

    return Vector3(right.x, right.y, right.z);
}

const Vector3 TransformComponent::GetUp() const noexcept
{
    const Quaternion& quat = worldTransform.GetRotation();
    XMVECTOR quatVec = XMVectorSet(quat.x, quat.y, quat.z, quat.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);
    XMVECTOR upVector = XMLoadFloat3(&Vector::up);

    upVector = XMVector3TransformNormal(upVector, rotationMatrix);
    upVector = XMVector3Normalize(upVector);

    XMFLOAT3 up;
    XMStoreFloat3(&up, upVector);

    return Vector3(up.x, up.y, up.z);
}

const Vector3 TransformComponent::GetForward() const noexcept
{
    const Quaternion& quat = worldTransform.GetRotation();
    XMVECTOR quatVec = XMVectorSet(quat.x, quat.y, quat.z, quat.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);
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

// Local Rotation 설정
void TransformComponent::SetLocalRotation(const Quaternion& rotation) noexcept
{
    localTransform.SetRotation(rotation); // 내부에서 정규화
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

// 로컬 위치/스케일 설정 함수들 (회전과 직접적 관련 없으므로 기존 로직 유지 또는 단순화)
void TransformComponent::SetLocalPosition(Position position) noexcept
{
    localTransform.SetPosition(position);

    // 월드 위치 업데이트
    if (HasParent())
    {
        XMVECTOR localPosVec = Vector::ConvertXMVECTOR(position);
        XMMATRIX parentWorldMatrix = parent->GetTransformMatrix();
        XMVECTOR worldPosVec = XMVector3TransformCoord(localPosVec, parentWorldMatrix);
        XMFLOAT3 worldPosFloat3;

        XMStoreFloat3(&worldPosFloat3, worldPosVec);
        worldTransform.SetPosition(worldPosFloat3);
    }

    else
        worldTransform.SetPosition(position);
}

void TransformComponent::SetLocalPosition(float x, float y, float z) noexcept
{
    SetLocalPosition(Position(x, y, z));
}

void TransformComponent::SetLocalScale(Scale scale) noexcept
{
    localTransform.SetScale(scale);

    // 월드 스케일 업데이트
    if (HasParent())
    {
        const Scale& parentScale = parent->GetScale();
        worldTransform.SetScale(scale.x * parentScale.x, scale.y * parentScale.y, scale.z * parentScale.z);
    }

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

// 회전 업데이트 헬퍼 함수
void TransformComponent::UpdateWorldRotation() noexcept
{
    if (HasParent())
    {
        const Quaternion& localRot = localTransform.GetRotation();
        const Quaternion& parentRot = parent->GetRotation();

        XMVECTOR localRotQ = XMVectorSet(localRot.x, localRot.y, localRot.z, localRot.w);
        XMVECTOR parentRotQ = XMVectorSet(parentRot.x, parentRot.y, parentRot.z, parentRot.w);

        // 월드 회전 = 부모 월드 회전 * 로컬 회전
        XMVECTOR worldRotQ = XMQuaternionMultiply(parentRotQ, localRotQ); // 순서 주의: Parent * Local
        worldRotQ = XMQuaternionNormalize(worldRotQ);

        Quaternion& worldQuat = worldTransform.GetRotation();
        worldQuat.x = XMVectorGetX(worldRotQ);
        worldQuat.y = XMVectorGetY(worldRotQ);
        worldQuat.z = XMVectorGetZ(worldRotQ);
        worldQuat.w = XMVectorGetW(worldRotQ);
    }

    else
        worldTransform.SetRotation(localTransform.GetRotation()); // 부모 없으면 로컬 = 월드
}

void TransformComponent::UpdateLocalRotation() noexcept
{
    if (HasParent())
    {
        const Quaternion& worldRot = worldTransform.GetRotation();
        const Quaternion& parentRot = parent->GetRotation();
        
        XMVECTOR worldRotQ = XMVectorSet(worldRot.x, worldRot.y, worldRot.z, worldRot.w);
        XMVECTOR parentRotQ = XMVectorSet(parentRot.x, parentRot.y, parentRot.z, parentRot.w);
        XMVECTOR parentRotInvQ = XMQuaternionInverse(parentRotQ);
        
        // 로컬 회전 = 부모 월드 회전의 역 * 월드 회전
        XMVECTOR localRotQ = XMQuaternionMultiply(parentRotInvQ, worldRotQ);
        localRotQ = XMQuaternionNormalize(localRotQ);

        Quaternion& localQuat = localTransform.GetRotation();
        localQuat.x = XMVectorGetX(localRotQ);
        localQuat.y = XMVectorGetY(localRotQ);
        localQuat.z = XMVectorGetZ(localRotQ);
        localQuat.w = XMVectorGetW(localRotQ);
    }

    else
        localTransform.SetRotation(worldTransform.GetRotation()); // 부모 없으면 월드 = 로컬
}

DirectX::XMMATRIX TransformComponent::GetTransformMatrix() const noexcept
{
    // 부모가 있는 경우: 로컬 변환 행렬 * 부모의 월드 변환 행렬
    if (HasParent())
    {
        XMMATRIX localMatrix = GetLocalTransformMatrix();
        XMMATRIX parentMatrix = parent->GetTransformMatrix();
        return localMatrix * parentMatrix;
    }

    // 부모가 없는 경우: 월드(이자 로컬) 변환 행렬
    else
    {
        const Quaternion& rot = worldTransform.GetRotation();
        XMVECTOR rotQuat = XMVectorSet(rot.x, rot.y, rot.z, rot.w);
        
        return
            XMMatrixScaling(worldTransform.GetScale().x, worldTransform.GetScale().y, worldTransform.GetScale().z) *
            XMMatrixRotationQuaternion(rotQuat) *
            XMMatrixTranslation(worldTransform.GetPosition().x, worldTransform.GetPosition().y, worldTransform.GetPosition().z);
    }
}

DirectX::XMFLOAT4X4& TransformComponent::GetTransformMatrix4x4() noexcept
{
    XMStoreFloat4x4(&transformMatrix, GetTransformMatrix());

    return transformMatrix;
}

DirectX::XMMATRIX TransformComponent::GetLocalTransformMatrix() const noexcept
{
    const Quaternion& rot = localTransform.GetRotation();
    XMVECTOR rotQuat = XMVectorSet(rot.x, rot.y, rot.z, rot.w);
    
    return
        XMMatrixScaling(localTransform.GetScale().x, localTransform.GetScale().y, localTransform.GetScale().z) *
        XMMatrixRotationQuaternion(rotQuat) *
        XMMatrixTranslation(localTransform.GetPosition().x, localTransform.GetPosition().y, localTransform.GetPosition().z);
}

DirectX::XMFLOAT4X4& TransformComponent::GetLocalTransformMatrix4x4() noexcept
{
    XMStoreFloat4x4(&transformMatrix, GetLocalTransformMatrix());
    return transformMatrix;
}

// 부모/자식 관련 함수들은 회전 방식 변경과 직접적인 관련이 없으므로 기존 코드 유지
void TransformComponent::SetParent(std::shared_ptr<TransformComponent> newParent) noexcept
{
    // 기존 부모로부터 자신을 제거 (만약 있다면)
    if (parent)
        parent->RemoveChild(shared_from_this());

    parent = newParent;

    if (parent)
    {
        parent->AddChild(shared_from_this());
        // 부모가 설정되면 로컬 변환을 기준으로 월드 변환을 다시 계산해야 합니다.
        // 또는 월드 변환을 유지하고 로컬 변환을 계산합니다.
        // 여기서는 월드 변환을 기준으로 로컬 변환을 업데이트합니다.
        SetPosition(worldTransform.GetPosition()); // 위치에 대한 로컬 업데이트
        SetRotation(worldTransform.GetRotation()); // 회전에 대한 로컬 업데이트
        SetScale(worldTransform.GetScale());       // 스케일에 대한 로컬 업데이트
    }

    // 부모가 제거되면 로컬 변환이 곧 월드 변환이 됩니다.
    else
        localTransform = worldTransform;
}

void TransformComponent::SetParent(std::shared_ptr<Object> parentObject) noexcept
{
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

    parent->RemoveChild(this->shared_from_this()); // shared_from_this() 사용
    parent = nullptr;
}

void TransformComponent::AddChild(std::shared_ptr<TransformComponent> child) noexcept
{
    if (!child) 
        return;

    // 이미 자식으로 있는지 확인
    for (const auto& c : children)
    {
        if (c == child) 
            return;
    }

    children.push_back(child);
    // childIndex 업데이트는 필요시 구현 (현재 코드에서는 이름 기반이므로 이름 가져와야 함)
    // child->SetParent(shared_from_this()); // 자식의 부모도 설정
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

    auto it = std::remove(children.begin(), children.end(), child);

    if (it != children.end())
        children.erase(it, children.end());
}

void TransformComponent::RemoveChild(UINT index) noexcept
{
    if (index >= children.size())
        return;

    // std::shared_ptr<TransformComponent> child = children[index];
    children.erase(children.begin() + index);
    // child->parent = nullptr;
    // childIndex 관련 로직도 업데이트 필요
}

void TransformComponent::RemoveChild(std::shared_ptr<Object> childObject) noexcept
{
    if (childObject)
        RemoveChild(childObject->GetComponent<TransformComponent>());
}

// childIndex를 사용하는 RemoveChild 함수들은 childIndex가 이름 기반이므로,
// 해당 부분의 로직은 그대로 두거나, TransformComponent의 ID 기반으로 변경해야 합니다.
// 여기서는 기존 로직을 유지합니다.
void TransformComponent::RemoveChild(std::string childObjectName) noexcept
{
    auto iter = childIndex.find(childObjectName);

    if (iter == childIndex.end()) 
        return;

    UINT indexToRemove = iter->second;
    // std::shared_ptr<TransformComponent> child = children[indexToRemove];
    children.erase(children.begin() + indexToRemove);
    childIndex.erase(iter);
    // child->parent = nullptr;

    for (size_t i = indexToRemove; i < children.size(); i++)
        childIndex[children[i]->GetObject()->GetName()] = (UINT)i; // GetObject()->GetName()이 유효해야 함
}

bool TransformComponent::HasChild(std::shared_ptr<TransformComponent> child) const noexcept
{
    if (!child) 
        return false;

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

void TransformComponent::UpdateTransform() noexcept
{
    // 부모가 있다면, 부모의 변환에 따라 자신의 월드 변환을 업데이트
    if (HasParent())
    {
        // 위치 업데이트
        XMVECTOR localPosVec = Vector::ConvertXMVECTOR(localTransform.GetPosition());
        XMMATRIX parentWorldMatrix = parent->GetTransformMatrix();
        XMVECTOR worldPosVec = XMVector3TransformCoord(localPosVec, parentWorldMatrix);

        Position& worldPos = worldTransform.GetPosition();
        worldPos.x = XMVectorGetX(worldPosVec);
        worldPos.y = XMVectorGetY(worldPosVec);
        worldPos.z = XMVectorGetZ(worldPosVec);

        // 회전 업데이트
        UpdateWorldRotation();

        // 스케일 업데이트
        const Scale& parentScale = parent->GetScale();
        const Scale& localScale = localTransform.GetScale();

        worldTransform.SetScale(localScale.x * parentScale.x, localScale.y * parentScale.y, localScale.z * parentScale.z);
    }

    // 부모가 없다면, 로컬 변환이 곧 월드 변환
    else
        worldTransform = localTransform;

    // 자식들의 변환도 업데이트
    for (const auto& child : children)
    {
        if (child != nullptr)
            child->UpdateTransform();
    }
}
