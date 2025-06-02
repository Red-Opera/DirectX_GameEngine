#include "stdafx.h"
#include "Transform.h"

#undef min
#undef max

// ============================
// 정적 상수 정의
// ============================

/// @brief Identity Transform 인스턴스 생성
const Transform Transform::identity = Transform();

// ============================
// 생성자 및 소멸자 구현
// ============================

/// @brief 기본 생성자 - Identity Transform으로 초기화
Transform::Transform() 
    : position(Vector3::zero), rotation(Quaternion::identity), scale(Vector3::one) 
{
    // 위치: (0,0,0), 회전: Identity Quaternion, 크기: (1,1,1)
}

/// @brief 매개변수 생성자
Transform::Transform(const Position& position, const Quaternion& rotation, const Scale& scale)
    : position(position), rotation(rotation), scale(scale) 
{
    // 전달받은 매개변수로 초기화
}

/// @brief 복사 생성자
Transform::Transform(const Transform& other)
    : position(other.position), rotation(other.rotation), scale(other.scale) 
{
    // 다른 Transform의 모든 멤버를 복사
}

/// @brief 이동 생성자 - 성능 최적화를 위한 이동 의미론
Transform::Transform(Transform&& other) noexcept
    : position(std::move(other.position)), rotation(std::move(other.rotation)), scale(std::move(other.scale)) 
{
    // 리소스를 이동하여 복사 오버헤드 방지
}

// ============================
// 연산자 오버로딩 구현
// ============================

/// @brief 복사 할당 연산자
Transform& Transform::operator=(const Transform& other)
{
    if (this != &other) // 자기 자신과의 할당 방지
    {
        position = other.position;
        rotation = other.rotation;
        scale = other.scale;
    }

    return *this;
}

/// @brief 이동 할당 연산자 - 성능 최적화를 위한 이동 의미론
Transform& Transform::operator=(Transform&& other) noexcept
{
    if (this != &other) // 자기 자신과의 할당 방지
    {
        position = std::move(other.position);
        rotation = std::move(other.rotation);
        scale = std::move(other.scale);
    }

    return *this;
}

/// @brief 동등 비교 연산자 - 모든 컴포넌트가 같은지 확인
bool Transform::operator==(const Transform& other) const
{
    return position == other.position && 
           rotation.x == other.rotation.x && rotation.y == other.rotation.y && 
           rotation.z == other.rotation.z && rotation.w == other.rotation.w &&
           scale == other.scale;
}

/// @brief 부등 비교 연산자
bool Transform::operator!=(const Transform& other) const
{
    return !(*this == other);
}

// ============================
// 위치 관련 메서드 구현
// ============================

/// @brief 위치를 Vector3로 설정
void Transform::SetPosition(const Position& newPosition)
{
    position = newPosition;
}

/// @brief 위치를 DirectX XMFLOAT3로 설정
void Transform::SetPosition(const XMFLOAT3& newPosition)
{
    position = { newPosition.x, newPosition.y, newPosition.z };
}

/// @brief 위치를 개별 float 값으로 설정
void Transform::SetPosition(float x, float y, float z)
{
    position = { x, y, z };
}

/// @brief 현재 위치 반환 (수정 가능)
Position& Transform::GetPosition()
{
    return position;
}

/// @brief 현재 위치 반환 (읽기 전용)
const Position& Transform::GetPosition() const
{
    return position;
}

// ============================
// 회전 관련 메서드 구현
// ============================

/// @brief Quaternion으로 회전 설정
void Transform::SetRotation(const Quaternion& newRotation)
{
    rotation = newRotation;
}

/// @brief 오일러 각도로 회전 설정 - 내부적으로 Quaternion으로 변환
void Transform::SetRotationFromEuler(const Euler& eulerAngles)
{
    rotation = Vector::ConvertQuaternion(eulerAngles);
}

/// @brief 개별 오일러 각도 값으로 회전 설정
void Transform::SetRotationFromEuler(float roll, float pitch, float yaw)
{
    rotation = Vector::ConvertQuaternion({ roll, pitch, yaw });
}

/// @brief 현재 회전을 Quaternion으로 반환 (수정 가능)
Quaternion& Transform::GetRotation()
{
    return rotation;
}

/// @brief 현재 회전을 Quaternion으로 반환 (읽기 전용)
const Quaternion& Transform::GetRotation() const
{
    return rotation;
}

/// @brief 현재 회전을 오일러 각도로 변환하여 반환
Vector3 Transform::GetRotationEuler() const
{
    return Vector::ConvertEuler(rotation);
}

// ============================
// 크기 관련 메서드 구현
// ============================

/// @brief 크기를 Vector3로 설정
void Transform::SetScale(const Scale& newScale)
{
    scale = newScale;
}

/// @brief 크기를 DirectX XMFLOAT3로 설정
void Transform::SetScale(const XMFLOAT3& newScale)
{
    scale = { newScale.x, newScale.y, newScale.z };
}

/// @brief 크기를 개별 float 값으로 설정
void Transform::SetScale(float x, float y, float z)
{
    scale = { x, y, z };
}

/// @brief 모든 축에 균일한 크기 설정
void Transform::SetScale(float uniform)
{
    scale = { uniform, uniform, uniform };
}

/// @brief 현재 크기 반환 (수정 가능)
Scale& Transform::GetScale()
{
    return scale;
}

/// @brief 현재 크기 반환 (읽기 전용)
const Scale& Transform::GetScale() const
{
    return scale;
}

// ============================
// 변환 행렬 관련 메서드 구현
// ============================

/// @brief 전체 변환 행렬 생성 (Scale * Rotation * Translation 순서)
XMMATRIX Transform::GetTransformMatrix() const
{
    return GetScaleMatrix() * GetRotationMatrix() * GetTranslationMatrix();
}

/// @brief 전체 변환 행렬을 XMFLOAT4X4 형식으로 반환
XMFLOAT4X4 Transform::GetTransformMatrix4x4() const
{
    XMFLOAT4X4 result;
    XMStoreFloat4x4(&result, GetTransformMatrix());

    return result;
}

/// @brief 이동 변환 행렬만 생성
XMMATRIX Transform::GetTranslationMatrix() const
{
    return XMMatrixTranslation(position.x, position.y, position.z);
}

/// @brief 회전 변환 행렬 생성 - Quaternion을 행렬로 변환
XMMATRIX Transform::GetRotationMatrix() const
{
    // Quaternion을 XMVECTOR로 변환 후 회전 행렬 생성
    XMVECTOR quatVec = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
    return XMMatrixRotationQuaternion(quatVec);
}

/// @brief 크기 변환 행렬만 생성
XMMATRIX Transform::GetScaleMatrix() const
{
    return XMMatrixScaling(scale.x, scale.y, scale.z);
}

// ============================
// 로컬 축 벡터 메서드 구현
// ============================

/// @brief 현재 회전이 적용된 오른쪽 방향 벡터 계산
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

/// @brief 현재 회전이 적용된 위쪽 방향 벡터 계산
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

/// @brief 현재 회전이 적용된 앞쪽 방향 벡터 계산
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

/// @brief 왼쪽 방향 벡터 반환 (오른쪽의 반대)
Vector3 Transform::GetLeft() const
{
    return GetRight() * -1.0f;
}

/// @brief 아래쪽 방향 벡터 반환 (위쪽의 반대)
Vector3 Transform::GetDown() const
{
    return GetUp() * -1.0f;
}

/// @brief 뒤쪽 방향 벡터 반환 (앞쪽의 반대)
Vector3 Transform::GetBack() const
{
    return GetForward() * -1.0f;
}

// ============================
// 변환 적용 메서드 구현
// ============================

/// @brief 현재 위치에 이동량 추가
void Transform::Translate(const Vector3& translation)
{
    position = position + translation;
}

/// @brief 현재 회전에 추가 회전 적용 - Quaternion 곱셈 사용
void Transform::Rotate(const Quaternion& additionalRotation)
{
    XMVECTOR currentQ = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
    XMVECTOR additionalQ = XMVectorSet(additionalRotation.x, additionalRotation.y, additionalRotation.z, additionalRotation.w);
    XMVECTOR resultQ = XMQuaternionMultiply(currentQ, additionalQ);
    resultQ = XMQuaternionNormalize(resultQ); // 정규화로 수치 오차 방지
    
    rotation.x = XMVectorGetX(resultQ);
    rotation.y = XMVectorGetY(resultQ);
    rotation.z = XMVectorGetZ(resultQ);
    rotation.w = XMVectorGetW(resultQ);
}

/// @brief 오일러 각도로 추가 회전 적용
void Transform::RotateEuler(const Euler& eulerAngles)
{
    Quaternion additionalQ = Vector::ConvertQuaternion(eulerAngles);
    Rotate(additionalQ);
}

/// @brief 지정된 대상을 바라보도록 회전 설정 (Look-At 변환)
void Transform::LookAt(const Vector3& target, const Vector3& upDir)
{
    Vector3 forward = target - position;

    if (forward.GetLength() < 0.001f) // 대상이 너무 가까우면 변환하지 않음
        return;
    
    // forward 벡터 정규화
    XMVECTOR forwardVec = Vector::ConvertXMVECTOR(forward);
    forwardVec = XMVector3Normalize(forwardVec);
    
    XMVECTOR upVec = Vector::ConvertXMVECTOR(upDir);
    upVec = XMVector3Normalize(upVec);
    
    // 외적으로 right 벡터 계산 (forward x up)
    XMVECTOR rightVec = XMVector3Cross(forwardVec, upVec);
    rightVec = XMVector3Normalize(rightVec);
    
    // up 벡터 재계산으로 직교성 보장 (right x forward)
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

// ============================
// 좌표계 변환 메서드 구현
// ============================

/// @brief 로컬 좌표의 점을 월드 좌표로 변환 (위치 영향 받음)
Vector3 Transform::TransformPoint(const Vector3& point) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMVECTOR pointVec = Vector::ConvertXMVECTOR(point);
    XMVECTOR transformed = XMVector3TransformCoord(pointVec, transform); // w=1로 처리
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);

    return Vector3(result.x, result.y, result.z);
}

/// @brief 로컬 좌표의 벡터를 월드 좌표로 변환 (위치 영향 안받음)
Vector3 Transform::TransformVector(const Vector3& vector) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMVECTOR vectorVec = Vector::ConvertXMVECTOR(vector);
    XMVECTOR transformed = XMVector3TransformNormal(vectorVec, transform); // w=0으로 처리
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);

    return Vector3(result.x, result.y, result.z);
}

/// @brief 월드 좌표의 점을 로컬 좌표로 변환
Vector3 Transform::InverseTransformPoint(const Vector3& point) const
{
    XMMATRIX transform = GetTransformMatrix();
    XMMATRIX inverse = XMMatrixInverse(nullptr, transform); // 역행렬 계산
    XMVECTOR pointVec = Vector::ConvertXMVECTOR(point);
    XMVECTOR transformed = XMVector3TransformCoord(pointVec, inverse);
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, transformed);

    return Vector3(result.x, result.y, result.z);
}

/// @brief 월드 좌표의 벡터를 로컬 좌표로 변환
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

// ============================
// 보간 메서드 구현 (정적)
// ============================

/// @brief 두 Transform 사이의 선형 보간 - 빠르지만 회전이 부자연스러울 수 있음
Transform Transform::Lerp(const Transform& from, const Transform& to, float t)
{
    t = std::max(0.0f, std::min(1.0f, t)); // t 값을 0~1 범위로 제한
    
    Transform result;
    result.position = from.position + (to.position - from.position) * t;
    
    // 쿼터니언 선형 보간 (Lerp)
    XMVECTOR fromQ = XMVectorSet(from.rotation.x, from.rotation.y, from.rotation.z, from.rotation.w);
    XMVECTOR toQ = XMVectorSet(to.rotation.x, to.rotation.y, to.rotation.z, to.rotation.w);
    XMVECTOR lerpedQ = XMVectorLerp(fromQ, toQ, t);
    lerpedQ = XMQuaternionNormalize(lerpedQ); // 정규화 필수
    
    result.rotation.x = XMVectorGetX(lerpedQ);
    result.rotation.y = XMVectorGetY(lerpedQ);
    result.rotation.z = XMVectorGetZ(lerpedQ);
    result.rotation.w = XMVectorGetW(lerpedQ);

    result.scale = from.scale + (to.scale - from.scale) * t;
    
    return result;
}

/// @brief 두 Transform 사이의 구면 선형 보간 - 더 부드러운 회전 결과
Transform Transform::Slerp(const Transform& from, const Transform& to, float t)
{
    t = std::max(0.0f, std::min(1.0f, t)); // t 값을 0~1 범위로 제한
    
    Transform result;
    result.position = from.position + (to.position - from.position) * t;
    result.scale = from.scale + (to.scale - from.scale) * t;
    
    // 쿼터니언 구면 선형 보간 (Slerp) - 각속도가 일정한 부드러운 회전
    XMVECTOR fromQuat = XMVectorSet(from.rotation.x, from.rotation.y, from.rotation.z, from.rotation.w);
    XMVECTOR toQuat = XMVectorSet(to.rotation.x, to.rotation.y, to.rotation.z, to.rotation.w);
    XMVECTOR slerpedQuat = XMQuaternionSlerp(fromQuat, toQuat, t);
    
    result.rotation.x = XMVectorGetX(slerpedQuat);
    result.rotation.y = XMVectorGetY(slerpedQuat);
    result.rotation.z = XMVectorGetZ(slerpedQuat);
    result.rotation.w = XMVectorGetW(slerpedQuat);
    
    return result;
}

// ============================
// 유틸리티 메서드 구현
// ============================

/// @brief 현재 Transform의 역변환 계산
Transform Transform::GetInverse() const
{
    XMMATRIX transformMat = GetTransformMatrix();
    XMMATRIX inverseMatrix = XMMatrixInverse(nullptr, transformMat);

    return FromMatrix(inverseMatrix);
}

/// @brief 4x4 행렬에서 Transform 추출 - 행렬 분해 사용
Transform Transform::FromMatrix(const XMFLOAT4X4& matrix)
{
    Transform result;
    XMMATRIX mat = XMLoadFloat4x4(&matrix);

    // DirectXMath의 행렬 분해 함수 사용
    XMVECTOR scaleVec, rotationQuat, translationVec;
    XMMatrixDecompose(&scaleVec, &rotationQuat, &translationVec, mat);

    // 위치 추출
    result.position.x = XMVectorGetX(translationVec);
    result.position.y = XMVectorGetY(translationVec);
    result.position.z = XMVectorGetZ(translationVec);
    
    // 회전 추출 (Quaternion)
    rotationQuat = XMQuaternionNormalize(rotationQuat);
    result.rotation.x = XMVectorGetX(rotationQuat);
    result.rotation.y = XMVectorGetY(rotationQuat);
    result.rotation.z = XMVectorGetZ(rotationQuat);
    result.rotation.w = XMVectorGetW(rotationQuat);
    
    // 크기 추출
    result.scale.x = XMVectorGetX(scaleVec);
    result.scale.y = XMVectorGetY(scaleVec);
    result.scale.z = XMVectorGetZ(scaleVec);
    
    return result;
}

/// @brief XMMATRIX에서 Transform 추출 - XMFLOAT4X4로 변환 후 처리
Transform Transform::FromMatrix(const XMMATRIX& matrix)
{
    XMFLOAT4X4 matrixFloat;
    XMStoreFloat4x4(&matrixFloat, matrix);

    return FromMatrix(matrixFloat);
}

/// @brief Transform을 기본값으로 리셋
void Transform::Reset()
{
    position = Vector3::zero;      // 위치: (0, 0, 0)
    rotation = Quaternion::identity; // 회전: Identity Quaternion (0, 0, 0, 1)
    scale = Vector3::one;          // 크기: (1, 1, 1)
}