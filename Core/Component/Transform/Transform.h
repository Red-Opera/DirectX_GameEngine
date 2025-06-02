#pragma once

#include "Utility/Vector.h" // Quaternion 정의 포함
#include <DirectXMath.h>

using namespace DirectX;

/**
 * @brief 3D 공간에서 객체의 위치, 회전, 크기를 관리하는 Transform 컴포넌트
 * 
 * Transform은 3D 그래픽스에서 객체의 공간적 변환을 담당하는 핵심 구조체입니다.
 * Quaternion을 사용하여 회전을 표현하며, DirectXMath 라이브러리와 호환됩니다.
 */
struct Transform final
{
    Position position;   ///< 3D 공간에서의 위치 (x, y, z)
    Quaternion rotation; ///< Quaternion으로 표현된 회전 정보
    Scale scale;         ///< 각 축에 대한 크기 비율 (x, y, z)

    // ============================
    // 생성자 및 소멸자
    // ============================
    
    /// @brief 기본 생성자 - Identity Transform으로 초기화
    Transform();
    
    /// @brief 매개변수 생성자
    /// @param position 초기 위치
    /// @param rotation 초기 회전 (Quaternion)
    /// @param scale 초기 크기
    Transform(const Position& position, const Quaternion& rotation, const Scale& scale);
    
    /// @brief 복사 생성자
    Transform(const Transform& other);
    
    /// @brief 이동 생성자
    Transform(Transform&& other) noexcept;

    // ============================
    // 연산자 오버로딩
    // ============================
    
    /// @brief 복사 할당 연산자
    Transform& operator=(const Transform& other);
    
    /// @brief 이동 할당 연산자
    Transform& operator=(Transform&& other) noexcept;

    /// @brief 동등 비교 연산자
    bool operator==(const Transform& other) const;
    
    /// @brief 부등 비교 연산자
    bool operator!=(const Transform& other) const;

    // ============================
    // 위치 관련 메서드
    // ============================
    
    /// @brief 위치를 설정합니다
    /// @param newPosition 새로운 위치
    void SetPosition(const Position& newPosition);
    
    /// @brief XMFLOAT3로 위치를 설정합니다
    /// @param newPosition DirectX XMFLOAT3 형식의 위치
    void SetPosition(const XMFLOAT3& newPosition);
    
    /// @brief 개별 좌표로 위치를 설정합니다
    /// @param x X 좌표
    /// @param y Y 좌표  
    /// @param z Z 좌표
    void SetPosition(float x, float y, float z);
    
    /// @brief 현재 위치를 반환합니다 (수정 가능)
    Position& GetPosition();
    
    /// @brief 현재 위치를 반환합니다 (읽기 전용)
    const Position& GetPosition() const;

    // ============================
    // 회전 관련 메서드
    // ============================
    
    /// @brief Quaternion으로 회전을 설정합니다
    /// @param newRotation 새로운 회전 (Quaternion)
    void SetRotation(const Quaternion& newRotation);
    
    /// @brief 오일러 각도로 회전을 설정합니다
    /// @param eulerAngles 오일러 각도 (Roll, Pitch, Yaw)
    void SetRotationFromEuler(const Euler& eulerAngles);
    
    /// @brief 개별 오일러 각도로 회전을 설정합니다
    /// @param roll X축 회전 (라디안)
    /// @param pitch Y축 회전 (라디안)
    /// @param yaw Z축 회전 (라디안)
    void SetRotationFromEuler(float roll, float pitch, float yaw);
    
    /// @brief 현재 회전을 Quaternion으로 반환합니다 (수정 가능)
    Quaternion& GetRotation();
    
    /// @brief 현재 회전을 Quaternion으로 반환합니다 (읽기 전용)
    const Quaternion& GetRotation() const;
    
    /// @brief 현재 회전을 오일러 각도로 반환합니다
    /// @return 오일러 각도 (Roll, Pitch, Yaw)
    Euler GetRotationEuler() const;

    // ============================
    // 크기 관련 메서드
    // ============================
    
    /// @brief 크기를 설정합니다
    /// @param newScale 새로운 크기
    void SetScale(const Scale& newScale);
    
    /// @brief XMFLOAT3로 크기를 설정합니다
    /// @param newScale DirectX XMFLOAT3 형식의 크기
    void SetScale(const XMFLOAT3& newScale);
    
    /// @brief 개별 축으로 크기를 설정합니다
    /// @param x X축 크기
    /// @param y Y축 크기
    /// @param z Z축 크기
    void SetScale(float x, float y, float z);
    
    /// @brief 모든 축에 균일한 크기를 설정합니다
    /// @param uniform 균일한 크기 값
    void SetScale(float uniform);
    
    /// @brief 현재 크기를 반환합니다 (수정 가능)
    Scale& GetScale();
    
    /// @brief 현재 크기를 반환합니다 (읽기 전용)
    const Scale& GetScale() const;

    // ============================
    // 변환 행렬 관련 메서드
    // ============================
    
    /// @brief 전체 변환 행렬을 반환합니다 (Scale * Rotation * Translation)
    /// @return XMMATRIX 형식의 변환 행렬
    XMMATRIX GetTransformMatrix() const;
    
    /// @brief 전체 변환 행렬을 XMFLOAT4X4로 반환합니다
    /// @return XMFLOAT4X4 형식의 변환 행렬
    XMFLOAT4X4 GetTransformMatrix4x4() const;
    
    /// @brief 이동 행렬만을 반환합니다
    /// @return 이동 변환 행렬
    XMMATRIX GetTranslationMatrix() const;
    
    /// @brief 회전 행렬만을 반환합니다
    /// @return 회전 변환 행렬
    XMMATRIX GetRotationMatrix() const;
    
    /// @brief 크기 행렬만을 반환합니다  
    /// @return 크기 변환 행렬
    XMMATRIX GetScaleMatrix() const;

    // ============================
    // 로컬 축 벡터 메서드
    // ============================
    
    /// @brief 현재 회전이 적용된 오른쪽 방향 벡터를 반환합니다
    /// @return 정규화된 오른쪽 방향 벡터
    Vector3 GetRight() const;
    
    /// @brief 현재 회전이 적용된 위쪽 방향 벡터를 반환합니다
    /// @return 정규화된 위쪽 방향 벡터
    Vector3 GetUp() const;
    
    /// @brief 현재 회전이 적용된 앞쪽 방향 벡터를 반환합니다
    /// @return 정규화된 앞쪽 방향 벡터
    Vector3 GetForward() const;
    
    /// @brief 현재 회전이 적용된 왼쪽 방향 벡터를 반환합니다
    /// @return 정규화된 왼쪽 방향 벡터 (-Right)
    Vector3 GetLeft() const;
    
    /// @brief 현재 회전이 적용된 아래쪽 방향 벡터를 반환합니다
    /// @return 정규화된 아래쪽 방향 벡터 (-Up)
    Vector3 GetDown() const;
    
    /// @brief 현재 회전이 적용된 뒤쪽 방향 벡터를 반환합니다
    /// @return 정규화된 뒤쪽 방향 벡터 (-Forward)
    Vector3 GetBack() const;

    // ============================
    // 변환 적용 메서드
    // ============================
    
    /// @brief 현재 위치에 이동량을 추가합니다
    /// @param translation 추가할 이동량
    void Translate(const Vector3& translation);

    /// @brief 현재 회전에 추가 회전을 적용합니다
    /// @param additionalRotation 추가할 회전 (Quaternion)
    void Rotate(const Quaternion& additionalRotation);
    
    /// @brief 현재 회전에 오일러 각도로 추가 회전을 적용합니다
    /// @param eulerAngles 추가할 회전 (오일러 각도)
    void RotateEuler(const Euler& eulerAngles);
    
    /// @brief 지정된 대상을 바라보도록 회전을 설정합니다
    /// @param target 바라볼 대상의 위치
    /// @param up 업 벡터 (기본값: Vector3::up)
    void LookAt(const Vector3& target, const Vector3& up = Vector3::up);

    // ============================
    // 좌표계 변환 메서드
    // ============================
    
    /// @brief 로컬 좌표의 점을 월드 좌표로 변환합니다
    /// @param point 로컬 좌표의 점
    /// @return 월드 좌표의 점
    Vector3 TransformPoint(const Vector3& point) const;
    
    /// @brief 로컬 좌표의 벡터를 월드 좌표로 변환합니다
    /// @param vector 로컬 좌표의 벡터
    /// @return 월드 좌표의 벡터
    Vector3 TransformVector(const Vector3& vector) const;
    
    /// @brief 월드 좌표의 점을 로컬 좌표로 변환합니다
    /// @param point 월드 좌표의 점
    /// @return 로컬 좌표의 점
    Vector3 InverseTransformPoint(const Vector3& point) const;
    
    /// @brief 월드 좌표의 벡터를 로컬 좌표로 변환합니다
    /// @param vector 월드 좌표의 벡터
    /// @return 로컬 좌표의 벡터
    Vector3 InverseTransformVector(const Vector3& vector) const;

    // ============================
    // 보간 메서드 (정적)
    // ============================
    
    /// @brief 두 Transform 사이의 선형 보간을 수행합니다
    /// @param from 시작 Transform
    /// @param to 끝 Transform
    /// @param t 보간 비율 (0.0f ~ 1.0f)
    /// @return 보간된 Transform
    static Transform Lerp(const Transform& from, const Transform& to, float t);
    
    /// @brief 두 Transform 사이의 구면 선형 보간을 수행합니다 (회전에 더 부드러운 결과)
    /// @param from 시작 Transform
    /// @param to 끝 Transform  
    /// @param t 보간 비율 (0.0f ~ 1.0f)
    /// @return 보간된 Transform
    static Transform Slerp(const Transform& from, const Transform& to, float t);

    // ============================
    // 유틸리티 메서드
    // ============================
    
    /// @brief 현재 Transform의 역변환을 반환합니다
    /// @return 역변환된 Transform
    Transform GetInverse() const;

    /// @brief 4x4 행렬에서 Transform을 추출합니다
    /// @param matrix XMFLOAT4X4 형식의 변환 행렬
    /// @return 추출된 Transform
    static Transform FromMatrix(const XMFLOAT4X4& matrix);
    
    /// @brief 4x4 행렬에서 Transform을 추출합니다
    /// @param matrix XMMATRIX 형식의 변환 행렬
    /// @return 추출된 Transform
    static Transform FromMatrix(const XMMATRIX& matrix);

    /// @brief Transform을 기본값(Identity)으로 리셋합니다
    /// Position: (0,0,0), Rotation: Identity Quaternion, Scale: (1,1,1)
    void Reset();

    // ============================
    // 정적 상수
    // ============================
    
    /// @brief Identity Transform 상수 (위치: 원점, 회전: 없음, 크기: 1)
    static const Transform identity;
};