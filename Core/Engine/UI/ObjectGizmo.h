#pragma once

#include "Core/Object/Object.h"
#include "Core/Component/Transform/TransformComponent.h"

#include "External/Imgui/imgui.h"
#include "External/Imgui/ImGuizmo.h"

#include <memory>

class Object;
class SceneGraph;

namespace Engine
{
    /**
     * @brief 3D 객체 변환을 위한 기즈모(Gizmo) 인터페이스 클래스
     * 
     * ImGuizmo 라이브러리를 사용하여 3D 공간에서 객체의 이동, 회전, 크기 조절을
     * 시각적으로 조작할 수 있는 기능을 제공합니다.
     * 
     * 쿼터니언 기반의 회전 처리로 짐벌락 현상을 방지합니다.
     */
    class ObjectGizmo
    {
    public:
        /// 싱글톤 인스턴스
        static std::shared_ptr<ObjectGizmo> instance;

        /**
         * @brief 싱글톤 인스턴스 반환
         * @return ObjectGizmo 싱글톤 인스턴스
         */
        static std::shared_ptr<ObjectGizmo> GetInstance()
        {
            if (!instance)
                instance = std::make_shared<ObjectGizmo>();

            return instance;
        }

        /**
         * @brief 기즈모 조작 대상 객체 설정
         * @param object 조작할 3D 객체. nullptr일 경우 기즈모 비활성화
         */
        void SetSelectedObject(std::shared_ptr<Object> object);

        /**
         * @brief 기즈모 설정을 위한 ImGui UI 렌더링
         * 
         * 다음 설정들을 제공합니다:
         * - 변환 유형 선택 (이동/회전/크기조절)
         * - 좌표계 선택 (월드/로컬)
         * - 스냅 설정
         */
        void RenderGizmoUI();

        /// 현재 기즈모 조작 유형 반환 (이동/회전/크기조절)
        ImGuizmo::OPERATION GetCurrentGizmoOperation() const { return currentGizmoSetting; }
        
        /// 현재 기즈모 좌표계 모드 반환 (월드/로컬)
        ImGuizmo::MODE GetCurrentGizmoMode() const { return currentApplyMode; }

        /**
         * @brief 지정된 객체에 기즈모 인터페이스 적용
         * @param object 조작할 객체
         * @param viewMatrix 카메라 뷰 행렬
         * @param projectionMatrix 투영 행렬
         * @param screenWidth 화면 너비
         * @param screenHeight 화면 높이
         */
        void ApplyGizmo
        (
            std::shared_ptr<Object> object,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projectionMatrix,
            float screenWidth,
            float screenHeight
        );

        /**
         * @brief 기즈모 시스템 업데이트
         * @param sceneGraph 씬 그래프 (선택된 객체 정보 포함)
         * @param viewMatrix 카메라 뷰 행렬
         * @param projMatrix 투영 행렬
         */
        void Update
        (
            const std::shared_ptr<SceneGraph>& sceneGraph,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projMatrix
        );
        
        /**
         * @brief 변환 행렬을 SRT 성분으로 분해 (쿼터니언 기반)
         * @param matrix 입력 변환 행렬 (4x4)
         * @param translation 출력: 위치 벡터 (x, y, z)
         * @param rotationQuat 출력: 회전 쿼터니언 (x, y, z, w)
         * @param scale 출력: 크기 벡터 (x, y, z)
         */
        void DecomposeMatrixToComponentsQuat(
            const float* matrix,
            float* translation,
            DirectX::XMFLOAT4* rotationQuat,
            float* scale);
            
        /**
         * @brief SRT 성분으로부터 변환 행렬 구성 (쿼터니언 기반)
         * @param matrix 출력: 변환 행렬 (4x4)
         * @param translation 입력: 위치 벡터 (x, y, z)
         * @param rotationQuat 입력: 회전 쿼터니언 (x, y, z, w)
         * @param scale 입력: 크기 벡터 (x, y, z)
         */
        void ComposeMatrixFromComponentsQuat(
            float* matrix,
            const float* translation,
            const DirectX::XMFLOAT4* rotationQuat,
            const float* scale);

    private:
        /// 현재 선택된 조작 대상 객체
        std::shared_ptr<Object> selectedObject = nullptr;

        /// 기즈모 좌표계 모드 (월드 좌표계/로컬 좌표계)
        ImGuizmo::MODE currentApplyMode = ImGuizmo::MODE::WORLD;
        
        /// 기즈모 조작 유형 (이동/회전/크기조절)
        ImGuizmo::OPERATION currentGizmoSetting = ImGuizmo::TRANSLATE;
    };
}