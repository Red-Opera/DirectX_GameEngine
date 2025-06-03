#include "stdafx.h"
#include "Camera.h"

#include "Core/Window.h"
#include "Core/DxGraphic.h"
#include "Core/Object/Object.h"

Camera::Camera(std::shared_ptr<class Object> object, bool isTethered) noexcept
    : Component(object), projection(1.0f, 9.0f / 16.0f, 0.5f, 400.0f), 
      indicator(), isTethered(isTethered)
{
    if (isTethered)
    {
        auto position = object->transform->GetPosition();

        indicator.SetPosition(position);
        projection.SetPosition(position);
    }

    Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
    auto pos = this->object->transform->GetPosition();
    DirectX::XMFLOAT3 position = { pos.x, pos.y, pos.z };
    auto rotation = this->object->transform->GetRotation();

    // 쿼터니언을 회전 행렬로 변환
    XMVECTOR quatVec = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);

    // 카메라의 앞 방향 Vector 계산
    const DirectX::XMVECTOR lookVector = XMVector3TransformNormal(Vector::forwardV, rotationMatrix);
    const DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);

    // 카메라가 바라보는 목표 위치
    const DirectX::XMVECTOR cameraTarget = positionVector + lookVector;

    // up 벡터도 회전 적용
    const DirectX::XMVECTOR upVector = XMVector3TransformNormal(Vector::upV, rotationMatrix);

    return DirectX::XMMatrixLookAtLH(positionVector, cameraTarget, upVector);
}

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
    return projection.GetMatrix();
}

void Camera::SpawnControlWidgets() noexcept
{
    auto& position = this->object->transform->GetPosition();
    auto& rotation = this->object->transform->GetRotation();

    // 프로젝션 설정은 유지
    projection.RenderWidgets();

    // 카메라 시각화 옵션도 유지
    ImGui::Checkbox("Enable Camera Indicator", &isEnableIndicator);
    ImGui::Checkbox("Enable Frustum Indicator", &isEnableFrustumIndicator);

    if (ImGui::Button("Reset"))
        Reset();

    // Transform이 외부(Inspector)에서 변경되었을 때 indicator와 projection 업데이트
    indicator.SetPosition(position);
    projection.SetPosition(position);

    // CameraIndicator와 CameraFrustum이 쿼터니언을 지원한다면 직접 전달
    indicator.SetRotation(rotation);
    projection.SetRotation(rotation);
}

void Camera::Reset() noexcept
{
    if (!isTethered)
    {
        auto& position = this->object->transform->GetPosition();
        position = { 0.0f, 0.0f, 0.0f };

        indicator.SetPosition(position);
        projection.SetPosition(position);
    }

    // 항등 쿼터니언으로 리셋
    auto& rotation = this->object->transform->GetRotation();
    rotation = Quaternion::identity; // 항등 쿼터니언

    // indicator와 projection도 항등 쿼터니언으로 설정
    indicator.SetRotation(rotation);
    projection.SetRotation(rotation);

    projection.Reset();
}

void Camera::Rotate(float dx, float dy) noexcept
{
    auto& rotation = this->object->transform->GetRotation();

    // 현재 쿼터니언을 오일러 각도로 변환 (Vector.h의 올바른 함수명 사용)
    Euler euler = Vector::ConvertEuler(rotation);

    // 오일러 각도 수정
    euler.x = std::clamp(euler.x + dy * rotationSpeed, 0.995f * -Math::PI / 2.0f, 0.995f * Math::PI / 2.0f);
    euler.y = Math::NormalizeRadian(euler.y + dx * rotationSpeed);

    // 수정된 오일러 각도를 쿼터니언으로 변환하여 설정
    rotation = Vector::ConvertQuaternion(euler);

    // indicator와 projection에 쿼터니언 직접 설정
    indicator.SetRotation(rotation);
    projection.SetRotation(rotation);
}

void Camera::Translate(Position translation) noexcept
{
    auto& position = this->object->transform->GetPosition();
    auto& rotation = this->object->transform->GetRotation();

    if (!isTethered)
    {
        DirectX::XMFLOAT3 translationFloat3 = { translation.x, translation.y, translation.z };

        // 쿼터니언을 회전 행렬로 변환
        XMVECTOR quatVec = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quatVec);

        // 이동 벡터에 회전과 이동 속도 적용
        XMVECTOR translationVec = XMLoadFloat3(&translationFloat3);
        translationVec = XMVector3TransformNormal(translationVec, rotationMatrix);
        translationVec = XMVectorScale(translationVec, moveSpeed);

        XMFLOAT3 transformedTranslation;
        XMStoreFloat3(&transformedTranslation, translationVec);

        position = { 
            position.x + transformedTranslation.x, 
            position.y + transformedTranslation.y, 
            position.z + transformedTranslation.z 
        };

        indicator.SetPosition(position);
        projection.SetPosition(position);
    }
}

void Camera::SetPosition(const Position& targetPosition) noexcept
{
    auto& position = this->object->transform->GetPosition();

    position = targetPosition;

    indicator.SetPosition(targetPosition);
    projection.SetPosition(targetPosition);
}

Position& Camera::GetPosition() const noexcept
{
    return this->object->transform->GetPosition();
}

const std::string Camera::GetName() const noexcept
{
    return object->GetName();
}

void Camera::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
    indicator.LinkTechniques(renderGraph);
    projection.LinkTechniques(renderGraph);
}

void Camera::Submit(size_t channel) const
{
    if (isEnableIndicator)
        indicator.Submit(channel);

    if (isEnableFrustumIndicator)
        projection.Submit(channel);
}

void Camera::RenderToGraphic() const
{
    Window::GetDxGraphic().SetCamera(GetMatrix());
    Window::GetDxGraphic().SetProjection(projection.GetMatrix());
}

void Camera::Initialize()
{

}

void Camera::Update(float deltaTime)
{
    Component::Update(deltaTime);
}

void Camera::LateUpdate()
{
    Component::LateUpdate();
}
