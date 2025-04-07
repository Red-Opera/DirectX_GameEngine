#include "stdafx.h"
#include "Inspector.h"

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

#include "External/Imgui/imgui.h"

#undef GetClassName

namespace Engine
{
	std::shared_ptr<Inspector> Inspector::instance = nullptr;
	std::shared_ptr<Object> Inspector::selectObject = nullptr;
	std::shared_ptr<Component> Inspector::selectComponent = nullptr;

	void Inspector::SetSelectObject(std::shared_ptr<Object> object) noexcept
	{
		if (selectObject == object)
			return;

		selectObject = object;
		selectComponent = nullptr;
	}

	void Inspector::ShowNodeInfomation() noexcept
	{
        if (ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoMove))
        {
            if (selectObject)
            {
                ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Selected Object: %s", selectObject->GetName().c_str());

                // 기본 정보 표시
                ImGui::Separator();
                ImGui::Text("Transform Information");

                // 위치 정보
                Position position = selectObject->transform->GetPosition();
                float pos[3] = { position.x, position.y, position.z };

                if (ImGui::DragFloat3("Position", pos, 0.1f))
                    selectObject->transform->SetPosition(pos[0], pos[1], pos[2]);

                // 회전 정보
                Rotation rotation = selectObject->transform->GetRotation();
				rotation.x = rotation.x * 180.0f / Math::PI;
				rotation.y = rotation.y * 180.0f / Math::PI;
				rotation.z = rotation.z * 180.0f / Math::PI;
                float rot[3] = { rotation.x, rotation.y, rotation.z };

				if (ImGui::DragFloat3("Rotation", rot, 0.1f))
				{
					rot[0] = Math::NormalizeAngle(rot[0]);
					rot[1] = Math::NormalizeAngle(rot[1]);
					rot[2] = Math::NormalizeAngle(rot[2]);

					rot[0] = Math::ConvertAngleToRadian(rot[0]);
					rot[1] = Math::ConvertAngleToRadian(rot[1]);
					rot[2] = Math::ConvertAngleToRadian(rot[2]);

                    selectObject->transform->SetRotation(rot[0], rot[1], rot[2]);
				}

                // 스케일 정보
                Scale scale = selectObject->transform->GetScale();
                float scl[3] = { scale.x, scale.y, scale.z };

                if (ImGui::DragFloat3("Scale", scl, 0.1f))
                    selectObject->transform->SetScale(scl[0], scl[1], scl[2]);

                // 컴포넌트 목록 표시
                ImGui::Separator();
                ImGui::Text("Components");

                const auto& components = selectObject->GetAllComponents();
                for (const auto& component : components)
                {
                    ImGui::PushID(component.get());  // 고유 ID 부여

                    // 컴포넌트 활성화/비활성화 체크박스
                    bool isActive = component->GetEnable();
                    if (ImGui::Checkbox("##ComponentActive", &isActive))
                        component->SetEnable(isActive);

                    ImGui::SameLine();

                    // 선택된 컴포넌트 하이라이트 표시
                    bool isSelected = (selectComponent == component);
                    if (isSelected)
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));

                    // 컴포넌트 이름을 버튼으로 표시하여 선택 가능하게 함
                    if (ImGui::Button(component->GetClassName().c_str(), ImVec2(-1, 0)))
                    {
                        selectComponent = component;
                    }

                    if (isSelected)
                        ImGui::PopStyleColor();

                    ImGui::PopID();
                }

                // 선택된 컴포넌트의 상세 정보 표시
                if (selectComponent)
                {
                    ImGui::Separator();
                    ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Component: %s", selectComponent->GetClassName().c_str());

                    // 컴포넌트 속성 표시 - 기본 속성만 표시
                    bool compIsActive = selectComponent->GetEnable();

                    if (ImGui::Checkbox("Component Active", &compIsActive))
                        selectComponent->SetEnable(compIsActive);

                    // TransformComponent인 경우 추가 정보 표시
                    if (selectComponent->GetClassName() == "TransformComponent")
                        ImGui::TextDisabled("Transform component is always required and cannot be removed.");

                    // 다른 컴포넌트 타입에 따른 특수 UI는 여기에 추가
                    else
                    {
                        if (ImGui::Button("Remove Component"))
                        {
                            selectObject->RemoveComponent(selectComponent->GetClassName());
                            selectComponent = nullptr;
                        }
                    }
                }

                // FolderViewInspector를 활용한 추가 정보 표시
                ImGui::Separator();
                ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Object Details");

                // 활성화 상태 표시 및 변경
                bool isActive = selectObject->GetActive();

                if (ImGui::Checkbox("Active", &isActive))
                    selectObject->SetActive(isActive);
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