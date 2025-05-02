#pragma once

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

#include "External/Imgui/imgui.h"
#include "External/Imgui/ImGuizmo.h"

#include <memory>

class Object;
class SceneGraph;

namespace Engine
{
    class ObjectGizmo
    {
    public:
		static std::shared_ptr<ObjectGizmo> instance;

		static std::shared_ptr<ObjectGizmo> GetInstance()
		{
            if (!instance)
                instance = std::make_unique<ObjectGizmo>();

			return instance;
		}

        void SetSelectedObject(std::shared_ptr<Object> object); // 객체 선택 설정
        void RenderGizmoUI();                                   // ImGui에서 조작 유형 선택을 위한 UI 렌더링

        // 게터 메서드 추가
        ImGuizmo::OPERATION GetCurrentGizmoOperation() const { return currentGizmoSetting; }
        ImGuizmo::MODE GetCurrentGizmoMode() const { return currentApplyMode; }

        // 직접 메인 화면에 렌더링하기 위한 함수 추가
        void ApplyGizmo
        (
            std::shared_ptr<Object> object,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projectionMatrix,
            float screenWidth,
            float screenHeight
        );

        void Update
        (
            const std::shared_ptr<SceneGraph>& sceneGraph,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projMatrix
        );

    private:
        // 현재 선택된 객체
        std::shared_ptr<Object> selectedObject = nullptr;

        ImGuizmo::MODE currentApplyMode = ImGuizmo::MODE::WORLD;                // ImGuizmo 작업 모드 (로컬 또는 월드)
        ImGuizmo::OPERATION currentGizmoSetting = ImGuizmo::TRANSLATE;          // ImGuizmo 조작 유형 (이동, 회전, 크기)
    };
}