#include "stdafx.h"
#include "SceneGraph.h"

#include "Core/Component/MeshComponent.h"
#include "Core/Component/TransformComponent.h"
#include "Core/Draw/Base/Drawable.h"
#include "Core/Draw/Mesh.h"
#include "Core/Draw/SceneGraphNode.h"
#include "Core/EngineUI/Inspector.h"
#include "Core/Object/Object.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/Technique.h"

SceneGraph::SceneGraph(std::string sceneName, std::vector<std::shared_ptr<Object>>& sceneObjects)
    : EngineLoop(), sceneName(sceneName), sceneObjects(sceneObjects)
{
    // 초기화 시 모든 오브젝트의 Outline 비활성화
    for (auto& object : sceneObjects)
        DisableOutlineForObject(object);
}

void SceneGraph::ShowNodeChildren(std::shared_ptr<Object> object) noexcept
{
    for (auto child : object->transform->GetChildrens())
    {
        // 자식이 없는 경우 ImGuiTreeNodeFlags_Leaf 플래그 사용
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

        if (child->GetChildCount() == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;

        // 노드가 선택되었는지 확인하고 선택된 경우 플래그 추가
        if (selectedObject && selectedObject == child->GetObject())
            flags |= ImGuiTreeNodeFlags_Selected;

        bool nodeOpen = ImGui::TreeNodeEx(child->GetObject()->GetName().c_str(), flags);

        // 노드 클릭 처리
        if (ImGui::IsItemClicked())
        {
            // 이전에 선택된 오브젝트가 있으면 Outline 비활성화
            if (selectedObject && selectedObject != child->GetObject())
                DisableOutlineForObject(selectedObject);

            // 새로운 오브젝트 선택 및 Outline 활성화
            selectedObject = child->GetObject();
            Engine::Inspector::GetInstance()->SetSelectObject(selectedObject);
            EnableOutlineForObject(selectedObject);
        }

        if (nodeOpen)
        {
            if (child->GetChildCount() > (size_t)0)
                ShowNodeChildren(child->GetObject());

            ImGui::TreePop();
        }
    }
}

void SceneGraph::UpdateSceneGraph() noexcept
{
    if (ImGui::Begin("Scene Graph", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        for (auto& object : sceneObjects)
        {
            // 루트 오브젝트도 자식이 없으면 ImGuiTreeNodeFlags_Leaf 플래그 사용
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

            if (object->transform->GetChildCount() == 0)
                flags |= ImGuiTreeNodeFlags_Leaf;

            // 노드가 선택되었는지 확인하고 선택된 경우 플래그 추가
            if (selectedObject && selectedObject == object)
                flags |= ImGuiTreeNodeFlags_Selected;

            if (ImGui::TreeNodeEx(object->GetName().c_str(), flags))
            {
                // 노드 클릭 처리
                if (ImGui::IsItemClicked())
                {
                    // 이전에 선택된 오브젝트가 있으면 Outline 비활성화
                    if (selectedObject && selectedObject != object)
                        DisableOutlineForObject(selectedObject);

                    // 새로운 오브젝트 선택 및 Outline 활성화
                    selectedObject = object;
                    Engine::Inspector::GetInstance()->SetSelectObject(object);
                    EnableOutlineForObject(selectedObject);
                }

                ShowNodeChildren(object);
                ImGui::TreePop();
            }

            else if (ImGui::IsItemClicked())
            {
                // 이전에 선택된 오브젝트가 있으면 Outline 비활성화
                if (selectedObject && selectedObject != object)
                    DisableOutlineForObject(selectedObject);

                // 새로운 오브젝트 선택 및 Outline 활성화
                selectedObject = object;
                Engine::Inspector::GetInstance()->SetSelectObject(object);
                EnableOutlineForObject(selectedObject);
            }
        }
    }

    ImGui::End();
}

// 코드 중복을 줄이기 위해 공통 함수를 템플릿 메서드로 구현
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

void SceneGraph::SetOutlineForSingleObject(std::shared_ptr<Object> object, bool enable) noexcept
{
    // Object의 모든 컴포넌트를 순회하며 Outline 설정
    auto components = object->GetAllComponents();

    for (auto& component : components)
    {
        // Drawable 컴포넌트 처리
        auto drawable = std::dynamic_pointer_cast<Drawable>(component);

        if (drawable)
            drawable->SetTechniqueActive("Outline", enable);

        // MeshComponent 처리
        auto meshComponent = std::dynamic_pointer_cast<MeshComponent>(component);

        if (meshComponent)
        {
            auto& meshes = meshComponent->GetMeshes();

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
    // 자식 오브젝트들을 모두 가져와서 각각 처리
    auto children = object->transform->GetChildrens();

    for (auto& childTransform : children)
    {
        auto childObject = childTransform->GetObject();

        if (childObject)
        {
            // 자식 오브젝트의 컴포넌트 처리
            SetOutlineForSingleObject(childObject, enable);

            // 자식의 자식도 재귀적으로 처리
            SetOutlineForChildren(childObject, enable);
        }
    }
}

void SceneGraph::Update()
{
    UpdateSceneGraph();
}