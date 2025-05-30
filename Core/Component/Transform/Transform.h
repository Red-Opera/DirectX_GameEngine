#pragma once

#include "Utility/Vector.h"
#include <DirectXMath.h>

using namespace DirectX;

struct Transform final
{
    Position position;
    Rotation rotation;
    Scale scale;

    Transform();
    Transform(const Position& position, const Rotation& rotation, const Scale& scale);
    Transform(const Transform& other);
    Transform(Transform&& other) noexcept;

    // 할당 연산자
    Transform& operator=(const Transform& other);
    Transform& operator=(Transform&& other) noexcept;

    // 비교 연산자
    bool operator==(const Transform& other) const;
    bool operator!=(const Transform& other) const;

    // 위치 설정/가져오기
    void SetPosition(const Position& newPosition);
    void SetPosition(const XMFLOAT3& newPosition);
    void SetPosition(float x, float y, float z);
    Position& GetPosition();
    const Position& GetPosition() const;

    // 회전 설정/가져오기
    void SetRotation(const Rotation& newRotation);
    void SetRotation(const XMFLOAT3& newRotation);
    void SetRotation(float x, float y, float z);
    Rotation& GetRotation();
    const Rotation& GetRotation() const;

    // 스케일 설정/가져오기
    void SetScale(const Scale& newScale);
    void SetScale(const XMFLOAT3& newScale);
    void SetScale(float x, float y, float z);
    void SetScale(float uniform);
    Scale& GetScale();
    const Scale& GetScale() const;

    // 변환 행렬 생성
    XMMATRIX GetTransformMatrix() const;
    XMFLOAT4X4 GetTransformMatrix4x4() const;
    
    // 개별 변환 행렬 생성
    XMMATRIX GetTranslationMatrix() const;
    XMMATRIX GetRotationMatrix() const;
    XMMATRIX GetScaleMatrix() const;

    // 로컬 축 벡터 가져오기 (회전 적용된)
    Vector3 GetRight() const;
    Vector3 GetUp() const;
    Vector3 GetForward() const;
    Vector3 GetLeft() const;
    Vector3 GetDown() const;
    Vector3 GetBack() const;

    // 이동 관련 메서드
    void Translate(const Vector3& translation);

    // 회전 관련 메서드
    void Rotate(const Vector3& eulerAngles);
    void LookAt(const Vector3& target, const Vector3& up = Vector3::up);

    // 점/벡터 변환
    Vector3 TransformPoint(const Vector3& point) const;
    Vector3 TransformVector(const Vector3& vector) const;
    Vector3 InverseTransformPoint(const Vector3& point) const;
    Vector3 InverseTransformVector(const Vector3& vector) const;

    // 보간 메서드
    static Transform Lerp(const Transform& from, const Transform& to, float t);
    static Transform Slerp(const Transform& from, const Transform& to, float t);

    // 역변환
    Transform GetInverse() const;

    // 행렬에서 Transform 추출
    static Transform FromMatrix(const XMFLOAT4X4& matrix);
    static Transform FromMatrix(const XMMATRIX& matrix);

    // 쿼터니언 관련
    static Vector3 QuaternionToEuler(const XMVECTOR& quaternion);
    static XMVECTOR EulerToQuaternion(const Vector3& euler);

    // 리셋
    void Reset();

    // 정적 상수
    static const Transform identity;
};