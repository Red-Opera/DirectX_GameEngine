#include "stdafx.h"
#include "SceneGraph.h"

#include "Core/Component/TransformComponent.h"
#include "Core/Draw/SceneGraphNode.h"
#include "Core/EngineUI/Inspector.h"
#include "Core/Object/Object.h"

SceneGraph::SceneGraph(std::string sceneName, std::vector<std::shared_ptr<Object>>& sceneObjects) 
	: EngineLoop(), sceneName(sceneName), sceneObjects(sceneObjects)
{

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
            selectedObject = child->GetObject();
			Engine::Inspector::GetInstance()->SetSelectObject(selectedObject);
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
    if (ImGui::Begin(sceneName.c_str()))
    {
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Scene Graph");

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
                    selectedObject = object;
					Engine::Inspector::GetInstance()->SetSelectObject(object);
                }

                ShowNodeChildren(object);
                ImGui::TreePop();
            }

            else if (ImGui::IsItemClicked())
            {
                selectedObject = object;
				Engine::Inspector::GetInstance()->SetSelectObject(object);
            }
        }
    }

    ImGui::End();
}

void SceneGraph::Update()
{
	UpdateSceneGraph();
}