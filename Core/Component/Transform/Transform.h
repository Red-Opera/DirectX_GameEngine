#pragma once

#include "Utility/Vector.h" // Quaternion 정의 포함
#include <DirectXMath.h>

using namespace DirectX;

struct Transform final
{
    Position position;
    Quaternion rotation; // Vector3에서 Quaternion으로 변경
    Scale scale;

    Transform();
    // 생성자에서 rotation 인자 타입을 Quaternion으로 변경
    Transform(const Position& position, const Quaternion& rotation, const Scale& scale);
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
    void SetRotation(const Quaternion& newRotation);                // 인수 타입을 Quaternion으로 변경
    void SetRotationFromEuler(const Euler& eulerAngles);            // 오일러 각도로 회전 설정하는 함수 추가
    void SetRotationFromEuler(float roll, float pitch, float yaw);  // 오일러 각도로 회전 설정하는 함수 추가
    Quaternion& GetRotation();                                      // 반환 타입을 Quaternion으로 변경
    const Quaternion& GetRotation() const;                          // 반환 타입을 Quaternion으로 변경
    Euler GetRotationEuler() const;                                 // 오일러 각도로 회전 가져오는 함수 추가

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
    void Rotate(const Quaternion& additionalRotation);                      // 쿼터니언으로 회전 추가
    void RotateEuler(const Euler& eulerAngles);                             // 오일러 각도로 회전 추가
    void LookAt(const Vector3& target, const Vector3& up = Vector3::up);

    // 점/벡터 변환
    Vector3 TransformPoint(const Vector3& point) const;
    Vector3 TransformVector(const Vector3& vector) const;
    Vector3 InverseTransformPoint(const Vector3& point) const;
    Vector3 InverseTransformVector(const Vector3& vector) const;

    // 보간 메서드
    static Transform Lerp(const Transform& from, const Transform& to, float t);
    static Transform Slerp(const Transform& from, const Transform& to, float t); // 내부 구현이 쿼터니언 직접 사용으로 변경됨

    // 역변환
    Transform GetInverse() const;

    // 행렬에서 Transform 추출
    static Transform FromMatrix(const XMFLOAT4X4& matrix); // 내부에서 쿼터니언 추출로 변경됨
    static Transform FromMatrix(const XMMATRIX& matrix); // 내부에서 쿼터니언 추출로 변경됨

    // 리셋
    void Reset();

    // 정적 상수
    static const Transform identity;
};