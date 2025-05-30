#include "stdafx.h"
#include "ObjectGizmo.h"

#include "Core/Scene/Base/SceneGraph.h"
#include "Core/Window.h"

std::shared_ptr<Engine::ObjectGizmo> Engine::ObjectGizmo::instance = nullptr;

void Engine::ObjectGizmo::SetSelectedObject(std::shared_ptr<Object> object)
{
	selectedObject = object;

	// 선택된 객체가 없으면 Gizmo를 비활성화
    if (selectedObject == nullptr)
    {
        currentGizmoSetting = ImGuizmo::TRANSLATE;
        currentApplyMode = ImGuizmo::LOCAL;
    }
}

void Engine::ObjectGizmo::RenderGizmoUI()
{
    if (ImGui::Begin("Gizmo Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Gizmo Mode:");

        // 변환 조작 유형 선택 (이동/회전/크기 조절)
        if (ImGui::RadioButton("Translate", currentGizmoSetting == ImGuizmo::TRANSLATE))
            currentGizmoSetting = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentGizmoSetting == ImGuizmo::ROTATE))
            currentGizmoSetting = ImGuizmo::ROTATE;

        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentGizmoSetting == ImGuizmo::SCALE))
            currentGizmoSetting = ImGuizmo::SCALE;

        // 로컬/월드 좌표계 선택
        ImGui::Text("Coordinate System:");

        if (ImGui::RadioButton("World", currentApplyMode == ImGuizmo::WORLD))
            currentApplyMode = ImGuizmo::WORLD;

        ImGui::SameLine();
        if (ImGui::RadioButton("Local", currentApplyMode == ImGuizmo::LOCAL))
            currentApplyMode = ImGuizmo::LOCAL;
    }

    ImGui::End();
}

void Engine::ObjectGizmo::ApplyGizmo
(
    std::shared_ptr<Object> object,
    const DirectX::XMMATRIX& viewMatrix,
    const DirectX::XMMATRIX& projectionMatrix,
    float screenWidth,
    float screenHeight
)
{
    if (object == nullptr)
        return;

    // ImGuizmo를 전체 화면에 설정
    ImGuizmo::Enable(true);
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

    // 전체 화면 크기 설정
    ImGuizmo::SetRect(0, 0, screenWidth, screenHeight);

    // 스냅 값 설정
    float snapValues[3] = { 1.0f, 1.0f, 1.0f };

    if (currentGizmoSetting == ImGuizmo::ROTATE)
        snapValues[0] = 15.0f;

    else if (currentGizmoSetting == ImGuizmo::SCALE)
        snapValues[0] = 0.1f;

    DirectX::XMFLOAT4X4 deltaMatrix;
    float* pDeltaMatrix = &deltaMatrix._11;

    DirectX::XMFLOAT4X4 viewMatrixFloat;
    DirectX::XMFLOAT4X4 projectionMatrixFloat;
    DirectX::XMStoreFloat4x4(&viewMatrixFloat, viewMatrix);
    DirectX::XMStoreFloat4x4(&projectionMatrixFloat, projectionMatrix);

    // Object의 현재 변환 행렬 가져오기
    auto transform = object->transform;

    // 항상 월드 변환 행렬을 사용하여 기즈모의 위치를 결정
    DirectX::XMFLOAT4X4 objectMatrix;
    DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetTransformMatrix());

    // 기즈모 조작 모드 설정 - 자식 오브젝트일 경우 로컬 모드와 상관없이 월드 위치에 표시
    ImGuizmo::MODE effectiveGizmoMode = currentApplyMode;

    bool manipulated = ImGuizmo::Manipulate
    (
        &viewMatrixFloat._11,
        &projectionMatrixFloat._11,
        currentGizmoSetting,
        effectiveGizmoMode,
        &objectMatrix._11,
        pDeltaMatrix,
        currentGizmoSetting != ImGuizmo::SCALE ? snapValues : nullptr
    );

    if (!manipulated)
        return;

    DirectX::XMVECTOR translation, rotation, scale;
    ImGuizmo::DecomposeMatrixToComponents(&objectMatrix._11, &translation.m128_f32[0], &rotation.m128_f32[0], &scale.m128_f32[0]);

    // 회전 값을 각도(도)로 반환하므로 라디안으로 변환
    Rotation rotationRadians;
    rotationRadians.x = Math::ConvertAngleToRadian(rotation.m128_f32[0]);
    rotationRadians.y = Math::ConvertAngleToRadian(rotation.m128_f32[1]);
    rotationRadians.z = Math::ConvertAngleToRadian(rotation.m128_f32[2]);

    if (currentApplyMode == ImGuizmo::WORLD)
    {
        transform->SetPosition(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]);
        transform->SetRotation(rotationRadians);
        transform->SetScale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]);
    }

    else
    {
        transform->SetLocalPosition(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]);
        transform->SetLocalRotation(rotationRadians);
        transform->SetLocalScale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]);
    }
}

void Engine::ObjectGizmo::Update
(
    const std::shared_ptr<::SceneGraph>& sceneGraph, 
    const DirectX::XMMATRIX& viewMatrix, 
    const DirectX::XMMATRIX& projMatrix
)
{
    // 컨트롤 UI는 별도 창에 표시
    RenderGizmoUI();

    auto selectedObject = sceneGraph->GetSelectedObject();

    // SceneGraph에서 선택된 객체가 있을 때 메인 화면에 Gizmo 렌더링
    if (selectedObject != nullptr)
    {
        // 화면 크기 가져오기
        float screenWidth = static_cast<float>(Window::GetDxGraphic().GetWidth());
        float screenHeight = static_cast<float>(Window::GetDxGraphic().GetHeight());

        // 메인 화면에 Gizmo 렌더링
        ApplyGizmo(selectedObject, viewMatrix, projMatrix, screenWidth, screenHeight);
    }
}