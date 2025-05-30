#include "stdafx.h"
#include "Transform.h"

#undef min
#undef max

const Transform Transform::identity = Transform();

Transform::Transform() 
    : position(Vector3::zero), rotation(Vector3::zero), scale(Vector3::one) 
{

}

Transform::Transform(const Position& position, const Rotation& rotation, const Scale& scale)
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
           rotation == other.rotation && 
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

void Transform::SetRotation(const Rotation& newRotation)
{
    rotation = newRotation;
}

void Transform::SetRotation(const XMFLOAT3& newRotation)
{
    rotation = { newRotation.x, newRotation.y, newRotation.z };
}

void Transform::SetRotation(float x, float y, float z)
{
    rotation = { x, y, z };
}

Rotation& Transform::GetRotation()
{
    return rotation;
}

const Rotation& Transform::GetRotation() const
{
    return rotation;
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
    return XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
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

void Transform::Rotate(const Vector3& eulerAngles)
{
    rotation = rotation + eulerAngles;
}

void Transform::LookAt(const Vector3& target, const Vector3& up)
{
    Vector3 forward = target - position;

    if (forward.GetLength() < 0.001f) 
        return;
    
    XMVECTOR forwardVec = Vector::ConvertXMVECTOR(forward);
    XMVECTOR upVec = Vector::ConvertXMVECTOR(up);
    XMVECTOR rightVec = XMVector3Cross(upVec, forwardVec);
    upVec = XMVector3Cross(forwardVec, rightVec);
    
    forwardVec = XMVector3Normalize(forwardVec);
    rightVec = XMVector3Normalize(rightVec);
    upVec = XMVector3Normalize(upVec);
    
    XMMATRIX lookMatrix = XMMatrixLookToLH(XMVectorZero(), forwardVec, upVec);
    XMFLOAT4X4 lookFloat;
    XMStoreFloat4x4(&lookFloat, lookMatrix);
    
    rotation = Vector::GetEulerAngle(lookFloat);
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
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp t to [0, 1]
    
    Transform result;
    result.position = from.position + (to.position - from.position) * t;
    result.rotation = from.rotation + (to.rotation - from.rotation) * t;
    result.scale = from.scale + (to.scale - from.scale) * t;
    
    return result;
}

Transform Transform::Slerp(const Transform& from, const Transform& to, float t)
{
    t = std::max(0.0f, std::min(1.0f, t)); // Clamp t to [0, 1]
    
    Transform result;
    result.position = from.position + (to.position - from.position) * t;
    result.scale = from.scale + (to.scale - from.scale) * t;
    
    // 쿼터니언 구면 선형 보간
    XMVECTOR fromQuat = EulerToQuaternion(from.rotation);
    XMVECTOR toQuat = EulerToQuaternion(to.rotation);
    XMVECTOR slerpedQuat = XMQuaternionSlerp(fromQuat, toQuat, t);
    result.rotation = QuaternionToEuler(slerpedQuat);
    
    return result;
}

Transform Transform::GetInverse() const
{
    XMMATRIX transform = GetTransformMatrix();
    XMMATRIX inverseMatrix = XMMatrixInverse(nullptr, transform);

    return FromMatrix(inverseMatrix);
}

Transform Transform::FromMatrix(const XMFLOAT4X4& matrix)
{
    Transform result;
    result.position = Vector::ConvertVector3(Vector::GetPosition(matrix));
    result.rotation = Vector::GetEulerAngle(matrix);
    
    // 스케일 추출
    result.scale.x = sqrtf(matrix._11 * matrix._11 + matrix._12 * matrix._12 + matrix._13 * matrix._13);
    result.scale.y = sqrtf(matrix._21 * matrix._21 + matrix._22 * matrix._22 + matrix._23 * matrix._23);
    result.scale.z = sqrtf(matrix._31 * matrix._31 + matrix._32 * matrix._32 + matrix._33 * matrix._33);
    
    return result;
}

Transform Transform::FromMatrix(const XMMATRIX& matrix)
{
    XMFLOAT4X4 matrixFloat;
    XMStoreFloat4x4(&matrixFloat, matrix);

    return FromMatrix(matrixFloat);
}

Vector3 Transform::QuaternionToEuler(const XMVECTOR& quaternion)
{
    // 쿼터니언에서 오일러 각 추출 (TransformComponent와 동일한 방식)
    XMMATRIX rotMatrix = XMMatrixRotationQuaternion(quaternion);
    float pitch = asinf(-rotMatrix.r[2].m128_f32[1]);
    float roll, yaw;

    if (cosf(pitch) > 0.0001f)
    {
        roll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
        yaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
    }

    else
    {
        roll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
        yaw = 0.0f;
    }

    return Vector3(roll, pitch, yaw);
}

XMVECTOR Transform::EulerToQuaternion(const Vector3& euler)
{
    return XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z);
}

void Transform::Reset()
{
    position = Vector3::zero;
    rotation = Vector3::zero;
    scale = Vector3::one;
}