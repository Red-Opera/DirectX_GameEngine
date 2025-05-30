#include "stdafx.h"
#include "SceneGraph.h"

#include "Core/Component/MeshComponent.h"
#include "Core/Component/Transform/TransformComponent.h"
#include "Core/Draw/Base/Drawable.h"
#include "Core/Draw/Mesh.h"
#include "Core/Draw/SceneGraphNode.h"
#include "Core/Engine/ObjectPicker.h"
#include "Core/Engine/UI/Inspector.h"
#include "Core/Object/Object.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/Technique.h"
#include "Core/Window.h"

SceneGraph::SceneGraph(std::string sceneName, std::vector<std::shared_ptr<Object>>& sceneObjects)
    : EngineLoop(), sceneName(sceneName), sceneObjects(sceneObjects)
{
    // 초기화 시 모든 오브젝트의 Outline 비활성화
    for (auto& object : sceneObjects)
        DisableOutlineForObject(object);

    // 부모 객체 캐시 초기화 (명시적으로 모든 관계 캐싱)
    InitializeParentCache();
}

void SceneGraph::ShowNodeChildren(std::shared_ptr<Object> object) noexcept
{
    const auto& children = object->transform->GetChildrens();

    // 자식 순회 시 복사 방지
    for (const auto& child : children)
    {
        auto childObject = child->GetObject();

        if (!childObject) 
            continue;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		// 자식이 없으면 Leaf 플래그 설정
        if (child->GetChildCount() == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;

		// 선택된 오브젝트와 비교하여 선택 플래그 설정
        if (selectedObject && selectedObject == childObject)
            flags |= ImGuiTreeNodeFlags_Selected;

        // 명시적으로 노드 상태 설정
        auto it = nodeOpenState.find(childObject);

        if (it != nodeOpenState.end())
            ImGui::SetNextItemOpen(it->second);

        bool nodeOpen = ImGui::TreeNodeEx(childObject->GetName().c_str(), flags);
        nodeOpenState[childObject] = nodeOpen;

        if (ImGui::IsItemClicked())
            SelectObject(childObject);

        if (nodeOpen)
        {
            if (child->GetChildCount() > 0)
                ShowNodeChildren(childObject);

            ImGui::TreePop();
        }
    }
}

void SceneGraph::UpdateSceneGraph() noexcept
{
    if (ImGui::Begin("Scene Graph", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        // 키 입력을 처리하기 위해 SceneGraph가 포커스를 가지고 있는지 확인
        bool isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

        // 키 입력 상태 재설정 (성능 향상)
        bool currentKeyDownPressed = isFocused && ImGui::IsKeyPressed(ImGuiKey_DownArrow, false);
        bool currentKeyUpPressed = isFocused && ImGui::IsKeyPressed(ImGuiKey_UpArrow, false);
        bool currentKeyRightPressed = isFocused && ImGui::IsKeyPressed(ImGuiKey_RightArrow, false);
        bool currentKeyLeftPressed = isFocused && ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false);

        // 선택된 노드에 대한 키 입력 처리
        if (selectedObject && isFocused)
        {
            if (currentKeyDownPressed && !keyDownPressed)
                SelectNextObjectInTree();

            else if (currentKeyUpPressed && !keyUpPressed)
                SelectPreviousObjectInTree();

            else if (currentKeyRightPressed && !keyRightPressed)
                HandleRightKeyPress();

            else if (currentKeyLeftPressed && !keyLeftPressed)
                HandleLeftKeyPress();

            // 키 상태 기록
            keyDownPressed = currentKeyDownPressed;
            keyUpPressed = currentKeyUpPressed;
            keyRightPressed = currentKeyRightPressed;
            keyLeftPressed = currentKeyLeftPressed;
        }

        // 노드 트리 렌더링
        for (auto& object : sceneObjects)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

            if (object->transform->GetChildCount() == 0)
                flags |= ImGuiTreeNodeFlags_Leaf;

            if (selectedObject && selectedObject == object)
                flags |= ImGuiTreeNodeFlags_Selected;

            // 명시적으로 노드 상태 설정
            auto it = nodeOpenState.find(object);

            // 노드 상태를 명시적으로 설정
            if (it != nodeOpenState.end())
                ImGui::SetNextItemOpen(it->second);

            bool nodeOpen = ImGui::TreeNodeEx(object->GetName().c_str(), flags);

            // 실제 열림 상태 반영
            nodeOpenState[object] = nodeOpen;

            if (ImGui::IsItemClicked())
                SelectObject(object);

            if (nodeOpen)
            {
                ShowNodeChildren(object);
                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}

// 객체 선택 공통 로직 - 중복 코드 제거
void SceneGraph::SelectObject(std::shared_ptr<Object> object)
{
    if (selectedObject && selectedObject != object)
        DisableOutlineForObject(selectedObject);

    selectedObject = object;
    Engine::Inspector::GetInstance()->SetSelectObject(selectedObject);

    EnableOutlineForObject(selectedObject);

    // 새 객체가 선택되면 ObjectGizmo에 알림
    if (objectGizmo != nullptr)
        objectGizmo->SetSelectedObject(selectedObject);
}

// 코드 중복을 줄이기 위해 공통 함수로 구현
void SceneGraph::SetOutlineForObject(std::shared_ptr<Object> object, bool enable) noexcept
{
    // 현재 오브젝트의 컴포넌트를 처리
    SetOutlineForSingleObject(object, enable);

    // 자식 오브젝트들도 recursive하게 처리
    SetOutlineForChildren(object, enable);
}

void SceneGraph::EnableOutlineForObject(std::shared_ptr<Object> object) noexcept
{
    SetOutlineForObject(object, true);
}

void SceneGraph::DisableOutlineForObject(std::shared_ptr<Object> object) noexcept
{
    SetOutlineForObject(object, false);
}

void SceneGraph::OnMouseClick(int x, int y, bool cursorEnabled)
{
    // 커서가 활성화되었을 때만 처리
    if (!cursorEnabled)
        return;

    // 현재 카메라 및 프로젝션 행렬 가져오기
    auto& graphic = Window::GetDxGraphic();
    DirectX::XMMATRIX viewMatrix = graphic.GetCamera();
    DirectX::XMMATRIX projMatrix = graphic.GetProjection();

    // 화면 크기 가져오기
    int screenWidth = graphic.GetWidth();
    int screenHeight = graphic.GetHeight();

    // 오브젝트 피커를 사용하여 클릭한 위치의 오브젝트 찾기
    auto pickedObject = Engine::ObjectPicker::Get().PickObjectAtScreenPoint(
        x, y, sceneObjects, viewMatrix, projMatrix, screenWidth, screenHeight);

    // 오브젝트가 선택되었으면 선택 처리
    if (pickedObject)
    {
        // 기존 선택된 오브젝트의 아웃라인 비활성화
        if (selectedObject)
            DisableOutlineForObject(selectedObject);

        // 새 오브젝트 선택 처리
        SelectObject(pickedObject);
    }
}

std::shared_ptr<Object> SceneGraph::GetSelectedObject() const noexcept
{
    return selectedObject;
}

void SceneGraph::SelectNextObjectInTree()
{
    // 현재 보이는 객체들만 수집
    std::vector<std::shared_ptr<Object>> visibleObjects;
    CollectVisibleObjects(visibleObjects);

    if (visibleObjects.empty())
        return;

    // 현재 선택된 객체가 없으면 첫 번째 객체를 선택
    if (!selectedObject)
    {
        SelectObject(visibleObjects[0]);
        return;
    }

    // 현재 선택된 객체 다음의 보이는 객체를 찾음
    for (size_t i = 0; i < visibleObjects.size() - 1; ++i)
    {
        if (visibleObjects[i] == selectedObject)
        {
            SelectObject(visibleObjects[i + 1]);
            return;
        }
    }
}

void SceneGraph::SelectPreviousObjectInTree()
{
    // 현재 보이는 객체들만 수집
    std::vector<std::shared_ptr<Object>> visibleObjects;
    CollectVisibleObjects(visibleObjects);

    if (visibleObjects.empty())
        return;

    // 현재 선택된 객체가 없으면 첫 번째 객체를 선택
    if (!selectedObject)
    {
        SelectObject(visibleObjects[0]);
        return;
    }

    // 현재 선택된 객체 이전의 보이는 객체를 찾음
    for (size_t i = 1; i < visibleObjects.size(); ++i)
    {
        if (visibleObjects[i] == selectedObject)
        {
            SelectObject(visibleObjects[i - 1]);
            return;
        }
    }
}

void SceneGraph::SetOutlineForSingleObject(std::shared_ptr<Object> object, bool enable) noexcept
{
    // Object의 모든 컴포넌트를 한 번만 가져오기
    const auto& components = object->GetAllComponents();

    for (const auto& component : components)
    {
        // dynamic_cast 최적화를 위해 타입 확인 먼저 수행
        if (auto drawable = std::dynamic_pointer_cast<Drawable>(component))
        {
            drawable->SetTechniqueActive("Outline", enable);
            continue;                                                           // 같은 컴포넌트가 두 타입 모두 상속받지 않으므로 계속 검사할 필요 없음
        }

        if (auto meshComponent = std::dynamic_pointer_cast<MeshComponent>(component))
        {
            const auto& meshes = meshComponent->GetMeshes();

            for (auto& mesh : meshes)
            {
                if (mesh)
                    mesh->SetTechniqueActive("Outline", enable);
            }
        }
    }
}

void SceneGraph::SetOutlineForChildren(std::shared_ptr<Object> object, bool enable) noexcept
{
    // 자식 오브젝트 참조 (복사 방지)
    const auto& children = object->transform->GetChildrens();

    for (const auto& childTransform : children)
    {
        auto childObject = childTransform->GetObject();
        if (childObject)
        {
            SetOutlineForSingleObject(childObject, enable);
            SetOutlineForChildren(childObject, enable);
        }
    }
}

void SceneGraph::CollectVisibleObjects(std::vector<std::shared_ptr<Object>>& visibleObjects)
{
    // 예상 용량 미리 할당으로 재할당 줄이기
    visibleObjects.reserve(sceneObjects.size() * 2);

    // 먼저 루트 오브젝트들을 추가
    for (const auto& rootObj : sceneObjects)
    {
        visibleObjects.push_back(rootObj);

        // 루트 오브젝트가 열려있을 때만 자식 수집
        if (IsNodeOpen(rootObj))
        {
            const auto& children = rootObj->transform->GetChildrens();
            for (const auto& childTransform : children)
            {
                auto childObject = childTransform->GetObject();
                if (!childObject) continue;

                visibleObjects.push_back(childObject);

                // 재귀적으로 열린 자식 노드들만 추가 (불필요한 객체 수집 방지)
                if (IsNodeOpen(childObject))
                {
                    std::function<void(const std::shared_ptr<Object>&)> collectOpenChildren 
                        = [&](const std::shared_ptr<Object>& parent) 
                    {
                        const auto& children = parent->transform->GetChildrens();

                        for (const auto& child : children) 
                        {
                            auto childObj = child->GetObject();

                            if (!childObj) 
                                continue;

                            visibleObjects.push_back(childObj);

                            if (IsNodeOpen(childObj))
                                collectOpenChildren(childObj);
                        }
                    };

                    collectOpenChildren(childObject);
                }
            }
        }
    }
}

void SceneGraph::CollectChildObjects(std::shared_ptr<Object> parent, std::vector<std::shared_ptr<Object>>& objects)
{
    const auto& children = parent->transform->GetChildrens();

    // 필요한 경우에만 예약 공간 확보
    if (objects.capacity() < objects.size() + children.size())
        objects.reserve(objects.size() + children.size());

    for (const auto& childTransform : children)
    {
        auto childObject = childTransform->GetObject();

        if (childObject)
        {
            objects.push_back(childObject);
            CollectChildObjects(childObject, objects);
        }
    }
}

inline bool SceneGraph::IsNodeOpen(const std::shared_ptr<Object>& object) const
{
    auto it = nodeOpenState.find(object);

    return it != nodeOpenState.end() && it->second;
}

std::shared_ptr<Object> SceneGraph::FindParentObject(const std::shared_ptr<Object>& childObject)
{
    auto it = parentCache.find(childObject);

    if (it != parentCache.end())
        return it->second;

    return nullptr;
}

void SceneGraph::HandleRightKeyPress()
{
    if (!selectedObject)
        return;

    // 자식이 있는 노드의 경우만 확장
    if (selectedObject->transform->GetChildCount() > 0)
    {
        // 열린 상태가 아닐 때만 처리
        if (!IsNodeOpen(selectedObject))
        {
            nodeOpenState[selectedObject] = true;
            // 다음 프레임에 반영
            ImGui::SetNextItemOpen(true);
        }
    }
}

void SceneGraph::HandleLeftKeyPress()
{
    if (!selectedObject)
        return;

    // 현재 선택된 객체가 루트 객체인지 확인 (부모가 없는 경우)
    bool isRootObject = false;

    for (const auto& rootObj : sceneObjects)
    {
        if (rootObj == selectedObject)
        {
            isRootObject = true;
            break;
        }
    }

    // 루트 객체가 아니라면 부모를 찾음
    if (!isRootObject)
    {
        // 캐시에서 부모 검색
        auto parentObject = FindParentObject(selectedObject);

        // 캐시에 없으면 직접 검색 (백업 방법)
        if (!parentObject)
        {
            parentObject = FindParentObjectDirect(selectedObject);

            // 찾은 부모를 캐시에 추가
            if (parentObject)
                parentCache[selectedObject] = parentObject;
        }

        // 부모가 있으면 부모로 이동
        if (parentObject)
        {
            // 이전 선택 해제
            DisableOutlineForObject(selectedObject);

            // 부모 선택
            selectedObject = parentObject;
            Engine::Inspector::GetInstance()->SetSelectObject(selectedObject);
            EnableOutlineForObject(selectedObject);

            // 부모 노드의 상태 업데이트 (접기)
            nodeOpenState[parentObject] = false;
            return;
        }
    }

    // 부모가 없는 경우 (루트 객체) 현재 노드를 접기
    nodeOpenState[selectedObject] = false;
}

void SceneGraph::InitializeParentCache()
{
    // 캐시 초기화
    parentCache.clear();

    // 루트 객체 처리
    for (auto& rootObj : sceneObjects)
    {
        // 루트 객체의 부모는 nullptr
        parentCache[rootObj] = nullptr;

        // 각 루트 객체의 모든 자식을 처리
        ProcessChildrenForCache(rootObj);
    }
}

void SceneGraph::ProcessChildrenForCache(const std::shared_ptr<Object>& parentObj)
{
    // 자식이 없으면 종료
    if (!parentObj || !parentObj->transform) 
        return;

	// 자식 객체들 가져오기
    const auto& children = parentObj->transform->GetChildrens();

    for (const auto& childTransform : children)
    {
        auto childObject = childTransform->GetObject();

        if (childObject)
        {
            // 자식의 부모를 명시적으로 설정
            parentCache[childObject] = parentObj;

            // 재귀적으로 이 자식의 모든 자식도 처리
            ProcessChildrenForCache(childObject);
        }
    }
}

std::shared_ptr<Object> SceneGraph::FindParentObjectDirect(const std::shared_ptr<Object>& childObject)
{
    // 모든 루트 객체 확인
    for (const auto& rootObj : sceneObjects)
    {
        // 직접 자식인지 확인
        const auto& rootChildren = rootObj->transform->GetChildrens();

        for (const auto& childTransform : rootChildren)
        {
            if (childTransform->GetObject() == childObject)
                return rootObj;
        }

        // 재귀적으로 더 깊은 레벨 확인
        auto result = FindParentInChildrenDirect(rootObj, childObject);

        if (result)
            return result;
    }

    return nullptr;
}

std::shared_ptr<Object> SceneGraph::FindParentInChildrenDirect(const std::shared_ptr<Object>& potentialParent, const std::shared_ptr<Object>& childObject)
{
    const auto& children = potentialParent->transform->GetChildrens();

    for (const auto& childTransform : children)
    {
        auto child = childTransform->GetObject();

        if (!child) 
            continue;

        // 이 자식의 자식들 확인
        const auto& grandChildren = child->transform->GetChildrens();

        for (const auto& grandChildTransform : grandChildren)
        {
            if (grandChildTransform->GetObject() == childObject)
                return child;
        }

        // 더 깊은 레벨 확인
        auto result = FindParentInChildrenDirect(child, childObject);

        if (result)
            return result;
    }

    return nullptr;
}


void SceneGraph::Update()
{
    UpdateSceneGraph();
}