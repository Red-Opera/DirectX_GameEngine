#include "stdafx.h"
#include "ObjectGizmo.h"

#include "Core/Scene/Base/SceneGraph.h"
#include "Core/Window.h"

// 싱글톤 인스턴스 초기화
std::shared_ptr<Engine::ObjectGizmo> Engine::ObjectGizmo::instance = nullptr;

void Engine::ObjectGizmo::SetSelectedObject(std::shared_ptr<Object> object)
{
    selectedObject = object;

    // 객체가 선택 해제되면 기즈모를 기본 상태로 리셋
    if (selectedObject == nullptr)
    {
        currentGizmoSetting = ImGuizmo::TRANSLATE;
        currentApplyMode = ImGuizmo::WORLD;
    }
}

void Engine::ObjectGizmo::RenderGizmoUI()
{
    // 기즈모 컨트롤 UI 윈도우 생성
    if (ImGui::Begin("Gizmo Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Gizmo Mode:");

        // 변환 조작 유형 선택 라디오 버튼
        if (ImGui::RadioButton("Translate", currentGizmoSetting == ImGuizmo::TRANSLATE))
            currentGizmoSetting = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentGizmoSetting == ImGuizmo::ROTATE))
            currentGizmoSetting = ImGuizmo::ROTATE;

        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentGizmoSetting == ImGuizmo::SCALE))
            currentGizmoSetting = ImGuizmo::SCALE;

        // 좌표계 선택 라디오 버튼
        ImGui::Text("Coordinate System:");

        if (ImGui::RadioButton("World", currentApplyMode == ImGuizmo::WORLD))
            currentApplyMode = ImGuizmo::WORLD;

        ImGui::SameLine();
        if (ImGui::RadioButton("Local", currentApplyMode == ImGuizmo::LOCAL))
            currentApplyMode = ImGuizmo::LOCAL;

        // 스냅 설정 섹션
        ImGui::Separator();
        ImGui::Text("Snap Settings:");

        static bool useSnap = false;
        ImGui::Checkbox("Use Snap", &useSnap);

        if (useSnap)
        {
            static float snapValue = 1.0f;
            
            // 변환 유형에 따른 스냅 값 설정
            if (currentGizmoSetting == ImGuizmo::TRANSLATE)
                ImGui::DragFloat("Translation Snap", &snapValue, 0.1f, 0.1f, 10.0f, "%.1f");

            else if (currentGizmoSetting == ImGuizmo::ROTATE)
            {
                snapValue = 15.0f; // 기본 회전 스냅: 15도
                ImGui::DragFloat("Rotation Snap (degrees)", &snapValue, 1.0f, 1.0f, 90.0f, "%.0f°");
            }

            else if (currentGizmoSetting == ImGuizmo::SCALE)
            {
                snapValue = 0.1f; // 기본 스케일 스냅: 0.1 단위
                ImGui::DragFloat("Scale Snap", &snapValue, 0.01f, 0.01f, 1.0f, "%.2f");
            }
        }
    }

    ImGui::End();
}

void Engine::ObjectGizmo::DecomposeMatrixToComponentsQuat(
    const float* matrix,
    float* translation,
    DirectX::XMFLOAT4* rotationQuat,
    float* scale)
{
    // float 배열을 DirectX 행렬로 로드
    DirectX::XMMATRIX matTransform = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)matrix);
    
    // DirectX 내장 함수를 사용하여 행렬을 Scale, Rotation, Translation으로 분해
    DirectX::XMVECTOR S, R, T;
    DirectX::XMMatrixDecompose(&S, &R, &T, matTransform);
    
    // 분해된 결과를 출력 파라미터에 저장
    DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)translation, T);  // 위치
    DirectX::XMStoreFloat4(rotationQuat, R);                     // 회전 (쿼터니언)
    DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)scale, S);        // 크기
}

void Engine::ObjectGizmo::ComposeMatrixFromComponentsQuat(
    float* matrix,
    const float* translation,
    const DirectX::XMFLOAT4* rotationQuat,
    const float* scale)
{
    // 입력 파라미터를 DirectX 벡터로 로드
    DirectX::XMVECTOR S = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)scale);        // 크기
    DirectX::XMVECTOR R = DirectX::XMLoadFloat4(rotationQuat);                     // 회전 (쿼터니언)
    DirectX::XMVECTOR T = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)translation);  // 위치
    
    // SRT 성분을 아핀 변환 행렬로 결합
    // 변환 순서: Scale -> Rotation -> Translation
    DirectX::XMMATRIX matTransform = DirectX::XMMatrixAffineTransformation(S, DirectX::XMVectorZero(), R, T);
    
    // 결과 행렬을 float 배열에 저장
    DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)matrix, matTransform);
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

    // ImGuizmo 렌더링 환경 설정
    ImGuizmo::Enable(true);
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);                           // 원근 투영 사용
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());      // 배경에 렌더링
    ImGuizmo::SetRect(0, 0, screenWidth, screenHeight);        // 렌더링 영역 설정

    // 변환 유형별 스냅 값 설정
    static bool useSnap = false;
    static float snapValues[3] = { 1.0f, 1.0f, 1.0f };

    if (currentGizmoSetting == ImGuizmo::ROTATE)
        snapValues[0] = snapValues[1] = snapValues[2] = 15.0f;   // 회전: 15도 단위

    else if (currentGizmoSetting == ImGuizmo::SCALE)
        snapValues[0] = snapValues[1] = snapValues[2] = 0.1f;    // 크기: 0.1 단위

    else // TRANSLATE
        snapValues[0] = snapValues[1] = snapValues[2] = 1.0f;    // 이동: 1.0 단위

    // 델타 행렬 (변화량을 저장할 행렬)
    DirectX::XMFLOAT4X4 deltaMatrix;
    float* pDeltaMatrix = &deltaMatrix._11;

    // DirectX 행렬을 ImGuizmo에서 사용할 수 있는 float 배열로 변환
    DirectX::XMFLOAT4X4 viewMatrixFloat, projectionMatrixFloat;
    DirectX::XMStoreFloat4x4(&viewMatrixFloat, viewMatrix);
    DirectX::XMStoreFloat4x4(&projectionMatrixFloat, projectionMatrix);

    // 객체의 변환 컴포넌트 가져오기
    auto transform = object->transform;

    // 현재 좌표계 모드에 따른 변환 행렬 선택
    DirectX::XMFLOAT4X4 objectMatrix;

    if (currentApplyMode == ImGuizmo::WORLD)
        DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetTransformMatrix());      // 월드 변환 행렬

    else
        DirectX::XMStoreFloat4x4(&objectMatrix, transform->GetLocalTransformMatrix()); // 로컬 변환 행렬

    // ImGuizmo를 사용하여 사용자 입력 처리 및 변환 행렬 조작
    bool manipulated = ImGuizmo::Manipulate
    (
        &viewMatrixFloat._11,           // 뷰 행렬
        &projectionMatrixFloat._11,     // 투영 행렬
        currentGizmoSetting,            // 조작 유형 (이동/회전/크기)
        currentApplyMode,               // 좌표계 모드 (월드/로컬)
        &objectMatrix._11,              // 객체 변환 행렬 (입출력)
        pDeltaMatrix,                   // 변화량 행렬 (출력)
        useSnap ? snapValues : nullptr  // 스냅 설정
    );

    // 사용자가 기즈모를 조작하지 않았다면 종료
    if (!manipulated)
        return;

    // 조작된 행렬을 쿼터니언 기반 SRT 성분으로 분해
    float translation[3], scale[3];
    DirectX::XMFLOAT4 rotationQuat;
    
    DecomposeMatrixToComponentsQuat(
        &objectMatrix._11,  // 입력: 조작된 변환 행렬
        translation,        // 출력: 위치
        &rotationQuat,      // 출력: 회전 (쿼터니언)
        scale               // 출력: 크기
    );

    // DirectX 쿼터니언을 커스텀 쿼터니언 클래스로 변환
    Quaternion quaternion(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w);
    
    // 좌표계 모드에 따라 적절한 변환 함수 호출
    if (currentApplyMode == ImGuizmo::WORLD)
    {
        // 월드 좌표계에서의 변환 적용
        transform->SetPosition(translation[0], translation[1], translation[2]);
        transform->SetRotation(quaternion);  // 쿼터니언으로 회전 설정 (짐벌락 방지)
        transform->SetScale(scale[0], scale[1], scale[2]);
    }

    else
    {
        // 로컬 좌표계에서의 변환 적용
        transform->SetLocalPosition(translation[0], translation[1], translation[2]);
        transform->SetLocalRotation(quaternion);  // 쿼터니언으로 회전 설정 (짐벌락 방지)
        transform->SetLocalScale(scale[0], scale[1], scale[2]);
    }
}

void Engine::ObjectGizmo::Update
(
    const std::shared_ptr<::SceneGraph>& sceneGraph,
    const DirectX::XMMATRIX& viewMatrix,
    const DirectX::XMMATRIX& projMatrix
)
{
    // 기즈모 설정 UI 렌더링
    RenderGizmoUI();

    // 씬 그래프에서 현재 선택된 객체 가져오기
    auto selectedObject = sceneGraph->GetSelectedObject();

    // 선택된 객체가 있으면 기즈모 적용
    if (selectedObject != nullptr)
    {
        // 현재 윈도우 크기 가져오기
        float screenWidth = static_cast<float>(Window::GetDxGraphic().GetWidth());
        float screenHeight = static_cast<float>(Window::GetDxGraphic().GetHeight());
        
        // 선택된 객체에 기즈모 인터페이스 적용
        ApplyGizmo(selectedObject, viewMatrix, projMatrix, screenWidth, screenHeight);
    }
}