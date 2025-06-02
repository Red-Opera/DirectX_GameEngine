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
        currentApplyMode = ImGuizmo::WORLD;
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

        // 스냅 설정
        ImGui::Separator();
        ImGui::Text("Snap Settings:");

        static bool useSnap = false;
        ImGui::Checkbox("Use Snap", &useSnap);

        if (useSnap)
        {
            static float snapValue = 1.0f;
            
            if (currentGizmoSetting == ImGuizmo::TRANSLATE)
            {
                ImGui::DragFloat("Translation Snap", &snapValue, 0.1f, 0.1f, 10.0f, "%.1f");
            }
            else if (currentGizmoSetting == ImGuizmo::ROTATE)
            {
                snapValue = 15.0f; // 기본 회전 스냅 값
                ImGui::DragFloat("Rotation Snap (degrees)", &snapValue, 1.0f, 1.0f, 90.0f, "%.0f°");
            }
            else if (currentGizmoSetting == ImGuizmo::SCALE)
            {
                snapValue = 0.1f; // 기본 스케일 스냅 값
                ImGui::DragFloat("Scale Snap", &snapValue, 0.01f, 0.01f, 1.0f, "%.2f");
            }
        }
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
    static bool useSnap = false;
    static float snapValues[3] = { 1.0f, 1.0f, 1.0f };

    if (currentGizmoSetting == ImGuizmo::ROTATE)
    {
        snapValues[0] = snapValues[1] = snapValues[2] = 15.0f; // 회전은 15도 단위
    }
    else if (currentGizmoSetting == ImGuizmo::SCALE)
    {
        snapValues[0] = snapValues[1] = snapValues[2] = 0.1f; // 스케일은 0.1 단위
    }
    else // TRANSLATE
    {
        snapValues[0] = snapValues[1] = snapValues[2] = 1.0f; // 이동은 1 단위
    }

    DirectX::XMFLOAT4X4 deltaMatrix;
    float* pDeltaMatrix = &deltaMatrix._11;

    DirectX::XMFLOAT4X4 viewMatrixFloat;
    DirectX::XMFLOAT4X4 projectionMatrixFloat;
    DirectX::XMStoreFloat4x4(&viewMatrixFloat, viewMatrix);
    DirectX::XMStoreFloat4x4(&projectionMatrixFloat, projectionMatrix);

    // Object의 현재 변환 행렬 가져오기
    auto transform = object->transform;

    // 적절한 변환 행렬 선택
    DirectX::XMFLOAT4X4 objectMatrix;
    if (currentApplyMode == ImGuizmo::WORLD)
    {
        // 월드 모드: 월드 변환 행렬 사용
        DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetTransformMatrix());
    }
    else
    {
        // 로컬 모드: 로컬 변환 행렬 사용
        DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetLocalTransformMatrix());
    }

    bool manipulated = ImGuizmo::Manipulate
    (
        &viewMatrixFloat._11,
        &projectionMatrixFloat._11,
        currentGizmoSetting,
        currentApplyMode,
        &objectMatrix._11,
        pDeltaMatrix,
        useSnap ? snapValues : nullptr
    );

    if (!manipulated)
        return;

    // ImGuizmo에서 반환된 행렬을 분해
    DirectX::XMVECTOR translation, rotation, scale;
    ImGuizmo::DecomposeMatrixToComponents(
        &objectMatrix._11, 
        &translation.m128_f32[0], 
        &rotation.m128_f32[0], 
        &scale.m128_f32[0]
    );

    // ImGuizmo는 회전을 오일러 각도(도)로 반환하므로 라디안으로 변환 후 쿼터니언으로 변환
    Euler eulerRadians;
    eulerRadians.x = Math::ConvertAngleToRadian(rotation.m128_f32[0]);
    eulerRadians.y = Math::ConvertAngleToRadian(rotation.m128_f32[1]);
    eulerRadians.z = Math::ConvertAngleToRadian(rotation.m128_f32[2]);

    // 오일러 각도를 쿼터니언으로 변환
    Quaternion rotationQuaternion = Vector::ConvertQuaternion(eulerRadians);

    // 모드에 따라 적절한 Transform 설정 함수 호출
    if (currentApplyMode == ImGuizmo::WORLD)
    {
        // 월드 모드: 월드 좌표계 기준으로 설정
        transform->SetPosition(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]);
        transform->SetRotation(rotationQuaternion);
        transform->SetScale(scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2]);
    }
    else
    {
        // 로컬 모드: 로컬 좌표계 기준으로 설정
        transform->SetLocalPosition(translation.m128_f32[0], translation.m128_f32[1], translation.m128_f32[2]);
        transform->SetLocalRotation(rotationQuaternion);
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