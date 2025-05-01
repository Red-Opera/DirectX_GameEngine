#include "stdafx.h"
#include "ObjectGizmo.h"

void Engine::ObjectGizmo::SetSelectedObject(std::shared_ptr<Object> object)
{
	selectedObject = object;

	// 선택된 객체가 없으면 Gizmo를 비활성화
    if (selectedObject == nullptr)
    {
        currentGizmoOperation = ImGuizmo::TRANSLATE;
        currentGizmoMode = ImGuizmo::LOCAL;

        return;
    }
}

bool Engine::ObjectGizmo::ApplyImGuizmoToObject
(
    std::shared_ptr<Object> selectedObject, 
    const DirectX::XMMATRIX& viewMatrix, 
    const DirectX::XMMATRIX& projectionMatrix
)
{
    if (!selectedObject)
        return false;

    // ImGui 창에서 ImGuizmo를 활성화할 영역 설정
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();

    // ImGui 윈도우의 내용 영역 시작점과 크기를 가져옴
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    // ImGuizmo가 그려질 영역 설정
    ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

    // Object의 현재 변환 행렬 가져오기
    auto transform = selectedObject->transform;
    DirectX::XMFLOAT4X4 objectMatrix;
    DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetTransformMatrix());

    // 스냅 값 설정 (그리드에 맞춰 변환 제어)
    float snapValues[3] = { 1.0f, 1.0f, 1.0f }; // 이동에 대한 스냅 값

    if (currentGizmoOperation == ImGuizmo::ROTATE)
        snapValues[0] = 15.0f; // 회전 각도 스냅 (15도 단위)

    else if (currentGizmoOperation == ImGuizmo::SCALE)
        snapValues[0] = 0.1f; // 크기 조절 스냅 (0.1 단위)

    // 변환 행렬을 저장할 변수
    DirectX::XMFLOAT4X4 deltaMatrix;
    float* pDeltaMatrix = &deltaMatrix._11;

    // 뷰 및 투영 행렬을 float 배열로 변환
    DirectX::XMFLOAT4X4 viewMatrixFloat;
    DirectX::XMFLOAT4X4 projectionMatrixFloat;
    DirectX::XMStoreFloat4x4(&viewMatrixFloat, viewMatrix);
    DirectX::XMStoreFloat4x4(&projectionMatrixFloat, projectionMatrix);

    // ImGuizmo를 사용하여 객체 조작
    bool manipulated = ImGuizmo::Manipulate(
        &viewMatrixFloat._11,       // 뷰 행렬
        &projectionMatrixFloat._11, // 투영 행렬
        currentGizmoOperation,      // 조작 유형 (이동, 회전, 크기)
        currentGizmoMode,           // 모드 (로컬 또는 월드)
        &objectMatrix._11,          // 객체의 변환 행렬
        pDeltaMatrix,               // 변화량 행렬
        currentGizmoOperation != ImGuizmo::SCALE ? snapValues : nullptr // 스냅 값 (크기에는 사용하지 않음)
    );

    if (manipulated)
    {
        // 조작된 행렬을 분해하여 위치, 회전, 크기로 변환
        DirectX::XMVECTOR translation, rotation, scale;
        ImGuizmo::DecomposeMatrixToComponents(&objectMatrix._11, &translation.m128_f32[0], &rotation.m128_f32[0], &scale.m128_f32[0]);

        // 월드 또는 로컬 모드에 따라 적절한 변환 적용
        if (currentGizmoMode == ImGuizmo::WORLD)
        {
            // 월드 공간에서의 변환
            transform->SetPosition(Position(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]));
            transform->SetRotation(Rotation(rotation.m128_f32[0], rotation.m128_f32[1], rotation.m128_f32[2]));
            transform->SetScale(Scale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]));
        }

        else
        {
            // 로컬 공간에서의 변환
            transform->SetLocalPosition(Position(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]));
            transform->SetLocalRotation(Rotation(rotation.m128_f32[0], rotation.m128_f32[1], rotation.m128_f32[2]));
            transform->SetLocalScale(Scale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]));
        }

        return true;
    }

    return false;
}

void Engine::ObjectGizmo::RenderGizmoUI()
{
    ImGui::Text("Gizmo Mode:");

    // 변환 조작 유형 선택 (이동/회전/크기 조절)
    if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
        currentGizmoOperation = ImGuizmo::TRANSLATE;

    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
        currentGizmoOperation = ImGuizmo::ROTATE;

    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
        currentGizmoOperation = ImGuizmo::SCALE;

    // 로컬/월드 좌표계 선택
    ImGui::Text("Coordinate System:");

    if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
        currentGizmoMode = ImGuizmo::WORLD;

    ImGui::SameLine();
    if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
        currentGizmoMode = ImGuizmo::LOCAL;
}

bool Engine::ObjectGizmo::ApplyImGuizmoToObjectOnMain(std::shared_ptr<Object> object, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, float screenWidth, float screenHeight)
{
    if (!object)
        return false;

    // ImGuizmo를 전체 화면에 설정
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList()); // 백그라운드 드로리스트 사용

    // 전체 화면 크기 설정
    ImGuizmo::SetRect(0, 0, screenWidth, screenHeight);

    // Object의 현재 변환 행렬 가져오기
    auto transform = object->transform;
    DirectX::XMFLOAT4X4 objectMatrix;
    DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetTransformMatrix());

    // 스냅 값 설정
    float snapValues[3] = { 1.0f, 1.0f, 1.0f };

    if (currentGizmoOperation == ImGuizmo::ROTATE)
        snapValues[0] = 15.0f;
    else if (currentGizmoOperation == ImGuizmo::SCALE)
        snapValues[0] = 0.1f;

    DirectX::XMFLOAT4X4 deltaMatrix;
    float* pDeltaMatrix = &deltaMatrix._11;

    DirectX::XMFLOAT4X4 viewMatrixFloat;
    DirectX::XMFLOAT4X4 projectionMatrixFloat;
    DirectX::XMStoreFloat4x4(&viewMatrixFloat, viewMatrix);
    DirectX::XMStoreFloat4x4(&projectionMatrixFloat, projectionMatrix);

    bool manipulated = ImGuizmo::Manipulate(
        &viewMatrixFloat._11,
        &projectionMatrixFloat._11,
        currentGizmoOperation,
        currentGizmoMode,
        &objectMatrix._11,
        pDeltaMatrix,
        currentGizmoOperation != ImGuizmo::SCALE ? snapValues : nullptr
    );

    if (manipulated)
    {
        DirectX::XMVECTOR translation, rotation, scale;
        ImGuizmo::DecomposeMatrixToComponents(&objectMatrix._11, &translation.m128_f32[0], &rotation.m128_f32[0], &scale.m128_f32[0]);

        if (currentGizmoMode == ImGuizmo::WORLD)
        {
            transform->SetPosition(Position(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]));
            transform->SetRotation(Rotation(rotation.m128_f32[0], rotation.m128_f32[1], rotation.m128_f32[2]));
            transform->SetScale(Scale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]));
        }
        else
        {
            transform->SetLocalPosition(Position(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]));
            transform->SetLocalRotation(Rotation(rotation.m128_f32[0], rotation.m128_f32[1], rotation.m128_f32[2]));
            transform->SetLocalScale(Scale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]));
        }

        return true;
    }

    return false;
}