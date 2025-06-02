#pragma once
#include "Transform.h" // Quaternion, Position, Scale, Transform 정의 포함

#include "Core/Object/EngineLoop.h"

#include "../Component.h"

#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief 3D 객체의 변환(Transform) 정보를 관리하는 컴포넌트 클래스
 * 
 * 이 클래스는 3D 공간에서 객체의 위치(Position), 회전(Rotation), 크기(Scale)를 
 * 관리합니다. 쿼터니언 기반의 회전 시스템을 사용하여 짐벌락 현상을 방지하며,
 * 부모-자식 계층 구조를 지원하여 복잡한 3D 씬 구성을 가능하게 합니다.
 * 
 * 월드 좌표계와 로컬 좌표계를 분리하여 관리하며, 부모 객체의 변환에 따라
 * 자식 객체의 월드 변환이 자동으로 계산됩니다.
 */
class TransformComponent : public Component, public std::enable_shared_from_this<TransformComponent>
{
public:
    /**
     * @brief TransformComponent 생성자
     * @param object 이 컴포넌트가 속할 3D 객체
     */
    TransformComponent(std::shared_ptr<class Object> object);
    virtual ~TransformComponent() = default;

    // =============================================
    // [World Transform - 월드 좌표계 변환]
    // =============================================

    /**
     * @brief 월드 좌표계에서의 위치 설정
     * @param position 설정할 월드 위치
     */
    void SetPosition(Position position) noexcept;
    
    /**
     * @brief 월드 좌표계에서의 위치 설정 (DirectX XMFLOAT3 타입)
     * @param position DirectX XMFLOAT3 형태의 위치 벡터
     */
    void SetPosition(DirectX::XMFLOAT3 position) noexcept;
    
    /**
     * @brief 월드 좌표계에서의 위치 설정 (개별 좌표값)
     * @param x X축 좌표
     * @param y Y축 좌표
     * @param z Z축 좌표
     */
    void SetPosition(float x, float y, float z) noexcept;

    /**
     * @brief 월드 좌표계에서의 위치 반환
     * @return 현재 월드 위치 참조
     */
    Position& GetPosition() noexcept;

    /**
     * @brief 월드 좌표계에서의 회전 설정 (쿼터니언)
     * @param rotation 설정할 회전 쿼터니언 (자동으로 정규화됨)
     */
    void SetRotation(const Quaternion& rotation) noexcept;
    
    /**
     * @brief 월드 좌표계에서의 회전 설정 (오일러 각도)
     * @param eulerAngles 오일러 각도 (라디안 단위)
     */
    void SetRotationFromEuler(const Euler& eulerAngles) noexcept;
    
    /**
     * @brief 월드 좌표계에서의 회전 설정 (개별 오일러 각도)
     * @param roll Z축 회전 (라디안)
     * @param pitch X축 회전 (라디안)
     * @param yaw Y축 회전 (라디안)
     */
    void SetRotationFromEuler(float roll, float pitch, float yaw) noexcept;

    /**
     * @brief 월드 좌표계에서의 회전 반환 (쿼터니언)
     * @return 현재 월드 회전 쿼터니언 참조
     */
    Quaternion& GetRotation() noexcept;
    
    /**
     * @brief 월드 좌표계에서의 회전 반환 (오일러 각도)
     * @return 현재 월드 회전을 오일러 각도로 변환한 값
     */
    Vector3 GetRotationEuler() const noexcept;

    /**
     * @brief 월드 좌표계에서의 크기 설정
     * @param scale 설정할 크기 벡터
     */
    void SetScale(Scale scale) noexcept;
    
    /**
     * @brief 월드 좌표계에서의 크기 설정 (DirectX XMFLOAT3 타입)
     * @param scale DirectX XMFLOAT3 형태의 크기 벡터
     */
    void SetScale(DirectX::XMFLOAT3 scale) noexcept;
    
    /**
     * @brief 월드 좌표계에서의 크기 설정 (개별 스케일값)
     * @param x X축 스케일
     * @param y Y축 스케일
     * @param z Z축 스케일
     */
    void SetScale(float x, float y, float z) noexcept;

    /**
     * @brief 월드 좌표계에서의 크기 반환
     * @return 현재 월드 크기 참조
     */
    Scale& GetScale() noexcept;

    /**
     * @brief 객체의 오른쪽 방향 벡터 반환 (월드 좌표계)
     * @return 정규화된 오른쪽 방향 벡터
     */
    const Vector3 GetRight() const noexcept;
    
    /**
     * @brief 객체의 위쪽 방향 벡터 반환 (월드 좌표계)
     * @return 정규화된 위쪽 방향 벡터
     */
    const Vector3 GetUp() const noexcept;
    
    /**
     * @brief 객체의 앞쪽 방향 벡터 반환 (월드 좌표계)
     * @return 정규화된 앞쪽 방향 벡터
     */
    const Vector3 GetForward() const noexcept;

    /**
     * @brief 월드 변환 객체 반환
     * @return Transform 객체 참조
     */
    Transform& GetTransform() noexcept;

    /**
     * @brief 월드 변환 행렬 반환 (DirectX XMMATRIX)
     * @return 4x4 변환 행렬
     */
    DirectX::XMMATRIX GetTransformMatrix() const noexcept;
    
    /**
     * @brief 월드 변환 행렬 반환 (DirectX XMFLOAT4X4)
     * @return 4x4 변환 행렬 참조 (캐시됨)
     */
    DirectX::XMFLOAT4X4 &GetTransformMatrix4x4() noexcept;

    // =============================================
    // [Local Transform - 로컬 좌표계 변환]
    // =============================================

    /**
     * @brief 로컬 좌표계에서의 위치 설정
     * @param position 설정할 로컬 위치 (부모 기준)
     */
    void SetLocalPosition(Position position) noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 위치 설정 (개별 좌표값)
     * @param x X축 좌표 (부모 기준)
     * @param y Y축 좌표 (부모 기준)
     * @param z Z축 좌표 (부모 기준)
     */
    void SetLocalPosition(float x, float y, float z) noexcept;

    /**
     * @brief 로컬 좌표계에서의 회전 설정 (쿼터니언)
     * @param rotation 설정할 로컬 회전 쿼터니언 (자동으로 정규화됨)
     */
    void SetLocalRotation(const Quaternion& rotation) noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 회전 설정 (오일러 각도)
     * @param eulerAngles 오일러 각도 (라디안 단위)
     */
    void SetLocalRotationFromEuler(const Euler& eulerAngles) noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 회전 설정 (개별 오일러 각도)
     * @param roll Z축 회전 (라디안)
     * @param pitch X축 회전 (라디안)
     * @param yaw Y축 회전 (라디안)
     */
    void SetLocalRotationFromEuler(float roll, float pitch, float yaw) noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 회전 반환 (쿼터니언)
     * @return 현재 로컬 회전 쿼터니언 참조
     */
    Quaternion& GetLocalRotation() noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 회전 반환 (오일러 각도)
     * @return 현재 로컬 회전을 오일러 각도로 변환한 값
     */
    Vector3 GetLocalRotationEuler() const noexcept;

    /**
     * @brief 로컬 좌표계에서의 크기 설정
     * @param scale 설정할 로컬 크기 벡터
     */
    void SetLocalScale(Scale scale) noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 크기 설정 (DirectX XMFLOAT3 타입)
     * @param scale DirectX XMFLOAT3 형태의 크기 벡터
     */
    void SetLocalScale(DirectX::XMFLOAT3 scale) noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 크기 설정 (개별 스케일값)
     * @param x X축 스케일
     * @param y Y축 스케일
     * @param z Z축 스케일
     */
    void SetLocalScale(float x, float y, float z) noexcept;

    /**
     * @brief 로컬 변환 객체 반환
     * @return Transform 객체 참조
     */
    Transform& GetLocalTransform() noexcept;

    /**
     * @brief 로컬 좌표계에서의 위치 반환
     * @return 현재 로컬 위치 참조
     */
    Position& GetLocalPosition() noexcept;
    
    /**
     * @brief 로컬 좌표계에서의 크기 반환
     * @return 현재 로컬 크기 참조
     */
    Scale& GetLocalScale() noexcept;

    /**
     * @brief 로컬 변환 행렬 반환 (DirectX XMMATRIX)
     * @return 4x4 로컬 변환 행렬
     */
    DirectX::XMMATRIX GetLocalTransformMatrix() const noexcept;
    
    /**
     * @brief 로컬 변환 행렬 반환 (DirectX XMFLOAT4X4)
     * @return 4x4 로컬 변환 행렬 참조 (캐시됨)
     */
    DirectX::XMFLOAT4X4& GetLocalTransformMatrix4x4() noexcept;

    /// 컴포넌트 클래스명 반환 (런타임)
    virtual std::string GetClassName() const override { return "TransformComponent"; };
    
    /// 컴포넌트 클래스명 반환 (컴파일타임)
    static std::string GetStaticClassName() { return "TransformComponent"; }

    // =============================================
    // [Hierarchy - 계층 구조 관리]
    // =============================================

    /**
     * @brief 부모 Transform 설정
     * @param parent 부모로 설정할 TransformComponent
     */
    void SetParent(std::shared_ptr<TransformComponent> parent) noexcept;
    
    /**
     * @brief 부모 객체 설정
     * @param parent 부모로 설정할 Object (TransformComponent를 가져야 함)
     */
    void SetParent(std::shared_ptr<Object> parent) noexcept;

    /**
     * @brief 부모 존재 여부 확인
     * @return 부모가 있으면 true, 없으면 false
     */
    bool HasParent() const noexcept;
    
    /**
     * @brief 부모 관계 해제
     * 현재 월드 변환을 유지하며 부모와의 관계를 끊습니다.
     */
    void RemoveParent() noexcept;

    /**
     * @brief 자식 Transform 추가
     * @param child 자식으로 추가할 TransformComponent
     */
    void AddChild(std::shared_ptr<TransformComponent> child) noexcept;
    
    /**
     * @brief 자식 객체 추가
     * @param child 자식으로 추가할 Object (TransformComponent를 가져야 함)
     */
    void AddChild(std::shared_ptr<Object> child) noexcept;

    /**
     * @brief 자식 Transform 제거
     * @param child 제거할 자식 TransformComponent
     */
    void RemoveChild(std::shared_ptr<TransformComponent> child) noexcept;
    
    /**
     * @brief 자식 객체 제거
     * @param child 제거할 자식 Object
     */
    void RemoveChild(std::shared_ptr<Object> child) noexcept;
    
    /**
     * @brief 이름으로 자식 객체 제거
     * @param childObjectName 제거할 자식 객체의 이름
     */
    void RemoveChild(std::string childObjectName) noexcept;
    
    /**
     * @brief 인덱스로 자식 객체 제거
     * @param index 제거할 자식의 인덱스
     */
    void RemoveChild(UINT index) noexcept;

    /**
     * @brief 자식 Transform 존재 여부 확인
     * @param child 확인할 자식 TransformComponent
     * @return 자식으로 존재하면 true
     */
    bool HasChild(std::shared_ptr<TransformComponent> child) const noexcept;
    
    /**
     * @brief 자식 객체 존재 여부 확인
     * @param child 확인할 자식 Object
     * @return 자식으로 존재하면 true
     */
    bool HasChild(std::shared_ptr<Object> child) const noexcept;
    
    /**
     * @brief 이름으로 자식 객체 존재 여부 확인
     * @param childObjectName 확인할 자식 객체의 이름
     * @return 자식으로 존재하면 true
     */
    bool HasChild(std::string childObjectName) const noexcept;
    
    /**
     * @brief 인덱스로 자식 객체 존재 여부 확인
     * @param index 확인할 인덱스
     * @return 해당 인덱스에 자식이 존재하면 true
     */
    bool HasChild(UINT index) const noexcept;

    /**
     * @brief 자식 Transform 가져오기 (객체 기준)
     * @param child 가져올 자식 Object
     * @return 자식 TransformComponent (없으면 nullptr)
     */
    std::shared_ptr<TransformComponent> GetChild(std::shared_ptr<Object> child) noexcept;
    
    /**
     * @brief 자식 Transform 가져오기 (이름 기준)
     * @param childObjectName 가져올 자식 객체의 이름
     * @return 자식 TransformComponent (없으면 nullptr)
     */
    std::shared_ptr<TransformComponent> GetChild(std::string childObjectName) noexcept;
    
    /**
     * @brief 자식 Transform 가져오기 (인덱스 기준)
     * @param index 가져올 자식의 인덱스
     * @return 자식 TransformComponent (없으면 nullptr)
     */
    std::shared_ptr<TransformComponent> GetChild(UINT index) noexcept;

    /**
     * @brief 모든 자식 Transform 목록 반환
     * @return 자식 TransformComponent 벡터
     */
    std::vector<std::shared_ptr<TransformComponent>> GetChildrens() noexcept;

    /**
     * @brief 자식 개수 반환
     * @return 현재 자식의 개수
     */
    size_t GetChildCount() const noexcept;

    /**
     * @brief 변환 업데이트 (계층 구조 전체)
     * 부모의 변환이 변경되었을 때 자식들의 월드 변환을 재계산합니다.
     */
    void UpdateTransform() noexcept;

private:
    /**
     * @brief 로컬 회전을 기반으로 월드 회전 업데이트
     * 부모의 회전과 로컬 회전을 결합하여 월드 회전을 계산합니다.
     */
    void UpdateWorldRotation() noexcept;
    
    /**
     * @brief 월드 회전을 기반으로 로컬 회전 업데이트
     * 월드 회전과 부모의 회전을 기반으로 로컬 회전을 계산합니다.
     */
    void UpdateLocalRotation() noexcept;

    /// 자식 객체 이름 -> 인덱스 매핑 (빠른 검색을 위함)
    std::unordered_map<std::string, UINT> childIndex;
    
    /// 자식 TransformComponent 목록
    std::vector<std::shared_ptr<TransformComponent>> children;
    
    /// 부모 TransformComponent
    std::shared_ptr<TransformComponent> parent;

    /// 변환 행렬 캐시 (성능 최적화용)
    DirectX::XMFLOAT4X4 transformMatrix;

    /// 월드 좌표계 변환 정보 (쿼터니언 기반 회전 포함)
    Transform worldTransform; 
    
    /// 로컬 좌표계 변환 정보 (쿼터니언 기반 회전 포함)
    Transform localTransform;
};