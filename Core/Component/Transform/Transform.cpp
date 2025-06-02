#include "stdafx.h"
#include "Transform.h"

#undef min
#undef max

const Transform Transform::identity = Transform();

Transform::Transform() 
    : position(Vector3::zero), rotation(Quaternion::identity), scale(Vector3::one) 
{

}

Transform::Transform(const Position& position, const Quaternion& rotation, const Scale& scale)
    : position(position), rotation(rotation), scale(scale) 
{

}

Transform::Transform(const Transform& other)
    : position(other.position), rotation(other.rotation), scale(other.scale) 
{

}

Transform::Transform(Transform&& other) noexcept
    : position(std::move(other.position)), rotation(std::move(other.rotation)), scale(std::move(other.scale)) 
{

}

Transform& Transform::operator=(const Transform& other)
{
    if (this != &other)
    {
        position = other.position;
        rotation = other.rotation;
        scale = other.scale;
    }

    return *this;
}

Transform& Transform::operator=(Transform&& other) noexcept
{
    if (this != &other)
    {
        position = std::move(other.position);
        rotation = std::move(other.rotation);
        scale = std::move(other.scale);
    }

    return *this;
}

bool Transform::operator==(const Transform& other) const
{
    return position == other.position && 
           rotation.x == other.rotation.x && rotation.y == other.rotation.y && rotation.z == other.rotation.z && rotation.w == other.rotation.w &&
           scale == other.scale;
}

bool Transform::operator!=(const Transform& other) const
{
    return !(*this == other);
}

void Transform::SetPosition(const Position& newPosition)
{
    position = newPosition;
}

void Transform::SetPosition(const XMFLOAT3& newPosition)
{
    position = { newPosition.x, newPosition.y, newPosition.z };
}

void Transform::SetPosition(float x, float y, float z)
{
    position = { x, y, z };
}

Position& Transform::GetPosition()
{
    return position;
}

const Position& Transform::GetPosition() const
{
    return position;
}

void Transform::SetRotation(const Quaternion& newRotation)
{
    rotation = newRotation;
}

void Transform::SetRotationFromEuler(const Euler& eulerAngles)
{
    rotation = Vector::ConvertQuaternion(eulerAngles);
}

void Transform::SetRotationFromEuler(float roll, float pitch, float yaw)
{
    rotation = Vector::ConvertQuaternion({ roll, pitch, yaw });
}

Quaternion& Transform::GetRotation()
{
    return rotation;
}

const Quaternion& Transform::GetRotation() const
{
    return rotation;
}

Vector3 Transform::GetRotationEuler() const
{
    return Vector::ConvertEuler(rotation);
}

void Transform::SetScale(const Scale& newScale)
{
    scale = newScale;
}

void Transform::SetScale(const XMFLOAT3& newScale)
{
    scale = { newScale.x, newScale.y, newScale.z };
}

void Transform::SetScale(float x, float y, float z)
{
    scale = { x, y, z };
}

void Transform::SetScale(float uniform)
{
    scale = { uniform, uniform, uniform };
}

Scale& Transform::GetScale()
{
    return scale;
}

const Scale& Transform::GetScale() const
{
    return scale;
}

XMMATRIX Transform::GetTransformMatrix() const
{
    return GetScaleMatrix() * GetRotationMatrix() * GetTranslationMatrix();
}

XMFLOAT4X4 Transform::GetTransformMatrix4x4() const
{
    XMFLOAT4X4 result;
    XMStoreFloat4x4(&result, GetTransformMatrix());

    return result;
}

XMMATRIX Transform::GetTranslationMatrix() const
{
    return XMMatrixTranslation(position.x, position.y, position.z);
}

XMMATRIX Transform::GetRotationMatrix() const
{
    // Quaternion을 XMVECTOR로 변환 후 회전 행렬 생성
    XMVECTOR quatVec = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
    return XMMatrixRotationQuaternion(quatVec);
}

XMMATRIX Transform::GetScaleMatrix() const
{
    return XMMatrixScaling(scale.x, scale.y, scale.z);
}

Vector3 Transform::GetRight() const
{
    XMMATRIX rotationMatrix = GetRotationMatrix();
    XMVECTOR rightVector = XMLoadFloat3(&Vector::right);
    rightVector = XMVector3TransformNormal(rightVector, rotationMatrix);
    rightVector = XMVector3Normalize(rightVector);

    XMFLOAT3 result;
    XMStoreFloat3(&result, rightVector);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::GetUp() const
{
    XMMATRIX rotationMatrix = GetRotationMatrix();
    XMVECTOR upVector = XMLoadFloat3(&Vector::up);
    upVector = XMVector3TransformNormal(upVector, rotationMatrix);
    upVector = XMVector3Normalize(upVector);

    XMFLOAT3 result;
    XMStoreFloat3(&result, upVector);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::GetForward() const
{
    XMMATRIX rotationMatrix = GetRotationMatrix();
    XMVECTOR forwardVector = XMLoadFloat3(&Vector::forward);
    forwardVector = XMVector3TransformNormal(forwardVector, rotationMatrix);
    forwardVector = XMVector3Normalize(forwardVector);

    XMFLOAT3 result;
    XMStoreFloat3(&result, forwardVector);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::GetLeft() const
{
    return GetRight() * -1.0f;
}

Vector3 Transform::GetDown() const
{
    return GetUp() * -1.0f;
}

Vector3 Transform::GetBack() const
{
    return GetForward() * -1.0f;
}

void Transform::Translate(const Vector3& translation)
{
    position = position + translation;
}

void Transform::Rotate(const Quaternion& additionalRotation)
{
    XMVECTOR currentQ = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
    XMVECTOR additionalQ = XMVectorSet(additionalRotation.x, additionalRotation.y, additionalRotation.z, additionalRotation.w);
    XMVECTOR resultQ = XMQuaternionMultiply(currentQ, additionalQ);
    resultQ = XMQuaternionNormalize(resultQ);
    
    rotation.x = XMVectorGetX(resultQ);
    rotation.y = XMVectorGetY(resultQ);
    rotation.z = XMVectorGetZ(resultQ);
    rotation.w = XMVectorGetW(resultQ);
}

void Transform::RotateEuler(const Euler& eulerAngles)
{
    Quaternion additionalQ = Vector::ConvertQuaternion(eulerAngles);
    Rotate(additionalQ);
}

void Transform::LookAt(const Vector3& target, const Vector3& upDir)
{
    Vector3 forward = target - position;
    if (forward.GetLength() < 0.001f) 
        return;
    
    // forward 벡터 정규화
    XMVECTOR forwardVec = Vector::ConvertXMVECTOR(forward);
    forwardVec = XMVector3Normalize(forwardVec);
    
    XMVECTOR upVec = Vector::ConvertXMVECTOR(upDir);
    upVec = XMVector3Normalize(upVec);
    
    // right 벡터 계산 (forward x up)
    XMVECTOR rightVec = XMVector3Cross(forwardVec, upVec);
    rightVec = XMVector3Normalize(rightVec);
    
    // up 벡터 재계산 (right x forward)
    upVec = XMVector3Cross(rightVec, forwardVec);
    
    // 회전 행렬 구성
    XMMATRIX rotationMatrix;
    rotationMatrix.r[0] = rightVec;
    rotationMatrix.r[1] = upVec;
    rotationMatrix.r[2] = forwardVec;
    rotationMatrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 행렬에서 쿼터니언 추출
    XMVECTOR quatVec = XMQuaternionRotationMatrix(rotationMatrix);
    quatVec = XMQuaternionNormalize(quatVec);
    
    rotation.x = XMVectorGetX(quatVec);
    rotation.y = XMVectorGetY(quatVec);
    rotation.z = XMVectorGetZ(quatVec);
    rotation.w = XMVectorGetW(quatVec);
}

Vector3 Transform::TransformPoint(const Vector3& point) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMVECTOR pointVec = Vector::ConvertXMVECTOR(point);
    XMVECTOR transformed = XMVector3TransformCoord(pointVec, transform);
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::TransformVector(const Vector3& vector) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMVECTOR vectorVec = Vector::ConvertXMVECTOR(vector);
    XMVECTOR transformed = XMVector3TransformNormal(vectorVec, transform);
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::InverseTransformPoint(const Vector3& point) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMMATRIX inverse = XMMatrixInverse(nullptr, transform);
    XMVECTOR pointVec = Vector::ConvertXMVECTOR(point);
    XMVECTOR transformed = XMVector3TransformCoord(pointVec, inverse);
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::InverseTransformVector(const Vector3& vector) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMMATRIX inverse = XMMatrixInverse(nullptr, transform);
    XMVECTOR vectorVec = Vector::ConvertXMVECTOR(vector);
    XMVECTOR transformed = XMVector3TransformNormal(vectorVec, inverse);
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);
    return Vector3(result.x, result.y, result.z);
}

Transform Transform::Lerp(const Transform& from, const Transform& to, float t)
{
    t = std::max(0.0f, std::min(1.0f, t)); 
    
    Transform result;
    result.position = from.position + (to.position - from.position) * t;
    
    // 쿼터니언 선형 보간
    XMVECTOR fromQ = XMVectorSet(from.rotation.x, from.rotation.y, from.rotation.z, from.rotation.w);
    XMVECTOR toQ = XMVectorSet(to.rotation.x, to.rotation.y, to.rotation.z, to.rotation.w);
    XMVECTOR lerpedQ = XMVectorLerp(fromQ, toQ, t);
    lerpedQ = XMQuaternionNormalize(lerpedQ);
    
    result.rotation.x = XMVectorGetX(lerpedQ);
    result.rotation.y = XMVectorGetY(lerpedQ);
    result.rotation.z = XMVectorGetZ(lerpedQ);
    result.rotation.w = XMVectorGetW(lerpedQ);

    result.scale = from.scale + (to.scale - from.scale) * t;
    
    return result;
}

Transform Transform::Slerp(const Transform& from, const Transform& to, float t)
{
    t = std::max(0.0f, std::min(1.0f, t)); 
    
    Transform result;
    result.position = from.position + (to.position - from.position) * t;
    result.scale = from.scale + (to.scale - from.scale) * t;
    
    // 쿼터니언 구면 선형 보간
    XMVECTOR fromQuat = XMVectorSet(from.rotation.x, from.rotation.y, from.rotation.z, from.rotation.w);
    XMVECTOR toQuat = XMVectorSet(to.rotation.x, to.rotation.y, to.rotation.z, to.rotation.w);
    XMVECTOR slerpedQuat = XMQuaternionSlerp(fromQuat, toQuat, t);
    
    result.rotation.x = XMVectorGetX(slerpedQuat);
    result.rotation.y = XMVectorGetY(slerpedQuat);
    result.rotation.z = XMVectorGetZ(slerpedQuat);
    result.rotation.w = XMVectorGetW(slerpedQuat);
    
    return result;
}

Transform Transform::GetInverse() const
{
    XMMATRIX transformMat = GetTransformMatrix();
    XMMATRIX inverseMatrix = XMMatrixInverse(nullptr, transformMat);
    return FromMatrix(inverseMatrix);
}

Transform Transform::FromMatrix(const XMFLOAT4X4& matrix)
{
    Transform result;
    XMMATRIX mat = XMLoadFloat4x4(&matrix);

    // 위치, 회전, 스케일 분해
    XMVECTOR scaleVec, rotationQuat, translationVec;
    XMMatrixDecompose(&scaleVec, &rotationQuat, &translationVec, mat);

    // 각 컴포넌트 저장
    result.position.x = XMVectorGetX(translationVec);
    result.position.y = XMVectorGetY(translationVec);
    result.position.z = XMVectorGetZ(translationVec);
    
    rotationQuat = XMQuaternionNormalize(rotationQuat);
    result.rotation.x = XMVectorGetX(rotationQuat);
    result.rotation.y = XMVectorGetY(rotationQuat);
    result.rotation.z = XMVectorGetZ(rotationQuat);
    result.rotation.w = XMVectorGetW(rotationQuat);
    
    result.scale.x = XMVectorGetX(scaleVec);
    result.scale.y = XMVectorGetY(scaleVec);
    result.scale.z = XMVectorGetZ(scaleVec);
    
    return result;
}

Transform Transform::FromMatrix(const XMMATRIX& matrix)
{
    XMFLOAT4X4 matrixFloat;
    XMStoreFloat4x4(&matrixFloat, matrix);
    return FromMatrix(matrixFloat);
}

void Transform::Reset()
{
    position = Vector3::zero;
    rotation = Quaternion::identity;
    scale = Vector3::one;
}