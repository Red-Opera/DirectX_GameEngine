#pragma once

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

#include "External/Imgui/imgui.h"
#include "External/Imgui/ImGuizmo.h"

#include <memory>

class Object;

namespace Engine
{
    class ObjectGizmo
    {
    public:
        // 객체 선택 설정
        void SetSelectedObject(std::shared_ptr<Object> object);

        // 선택된 객체에 ImGuizmo 적용
        bool ApplyImGuizmoToObject(std::shared_ptr<Object> selectedObject, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

        // ImGui에서 조작 유형 선택을 위한 UI 렌더링
        void RenderGizmoUI();

        // 게터 메서드 추가
        ImGuizmo::OPERATION GetCurrentGizmoOperation() const { return currentGizmoOperation; }
        ImGuizmo::MODE GetCurrentGizmoMode() const { return currentGizmoMode; }

        // 직접 메인 화면에 렌더링하기 위한 함수 추가
        bool ApplyImGuizmoToObjectOnMain
        (
            std::shared_ptr<Object> object,
            const DirectX::XMMATRIX& viewMatrix,
            const DirectX::XMMATRIX& projectionMatrix,
            float screenWidth,
            float screenHeight
        );

    private:
        // 현재 선택된 객체
        std::shared_ptr<Object> selectedObject = nullptr;

        ImGuizmo::MODE currentGizmoMode = ImGuizmo::MODE::WORLD;                // ImGuizmo 작업 모드 (로컬 또는 월드)
        ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;        // ImGuizmo 조작 유형 (이동, 회전, 크기)
    };
}