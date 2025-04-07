#include "stdafx.h"
#include "Inspector.h"

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

#include "External/Imgui/imgui.h"

#undef GetClassName

namespace Engine
{
	std::shared_ptr<Inspector> Inspector::instance = nullptr;
	std::shared_ptr<Object> Inspector::currentSelectObject = nullptr;

	void Inspector::SetSelectObject(std::shared_ptr<Object> object) noexcept
	{
		if (currentSelectObject == object)
			return;

		currentSelectObject = object;
	}

	void Inspector::ShowNodeInfomation() noexcept
	{
        if (ImGui::Begin("Inspector"))
        {
            if (currentSelectObject)
            {
                ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Selected Object: %s", currentSelectObject->GetName().c_str());

                // 기본 정보 표시
                ImGui::Separator();
                ImGui::Text("Transform Information");

                // 위치 정보
                auto position = currentSelectObject->transform->GetPosition();
                float pos[3] = { position.x, position.y, position.z };
                if (ImGui::DragFloat3("Position", pos, 0.1f))
                    currentSelectObject->transform->SetPosition(pos[0], pos[1], pos[2]);

                // 회전 정보
                auto rotation = currentSelectObject->transform->GetRotation();
                float rot[3] = { rotation.x, rotation.y, rotation.z };
                if (ImGui::DragFloat3("Rotation", rot, 0.1f))
                    currentSelectObject->transform->SetRotation(rot[0], rot[1], rot[2]);

                // 스케일 정보
                auto scale = currentSelectObject->transform->GetScale();
                float scl[3] = { scale.x, scale.y, scale.z };
                if (ImGui::DragFloat3("Scale", scl, 0.1f))
                    currentSelectObject->transform->SetScale(scl[0], scl[1], scl[2]);

                // 컴포넌트 목록 표시
                ImGui::Separator();
                ImGui::Text("Components");

                const auto& components = currentSelectObject->GetAllComponents();
                for (const auto& component : components)
                {
                    ImGui::BulletText("%s", component->GetClassName().c_str());
                }

                // FolderViewInspector를 활용한 추가 정보 표시
                ImGui::Separator();
                ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Object Details");

                // 활성화 상태 표시 및 변경
                bool isActive = currentSelectObject->GetActive();
                if (ImGui::Checkbox("Active", &isActive))
                    currentSelectObject->SetActive(isActive);
            }

            else
                ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "No object selected");
        }

        ImGui::End();
	}

	void Inspector::Initialize()
	{

	}

	void Inspector::BeforeFrame()
	{

	}

	void Inspector::Start()
	{

	}

	void Inspector::LateStart()
	{

	}

	void Inspector::Update()
	{
        ShowNodeInfomation();
	}

	void Inspector::LateUpdate()
	{

	}

	void Inspector::Finalize()
	{

	}

	void Inspector::Destroy()
	{

	}

	void Inspector::OnEnable()
	{

	}

	void Inspector::OnDisable()
	{

	}

	void Inspector::Reset()
	{

	}
}