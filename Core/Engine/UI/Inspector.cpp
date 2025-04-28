#include "stdafx.h"
#include "Inspector.h"

#include "Core/Camera/Camera.h"
#include "Core/Camera/CameraContainer.h"
#include "Core/Component/MeshComponent.h"
#include "Core/Component/TransformComponent.h"
#include "Core/Draw/Light/PointLight.h"
#include "Core/Draw/Model.h"
#include "Core/Draw/ModelEditor.h"
#include "Core/Draw/Object/ColorObject.h"
#include "Core/Draw/SceneGraphNode.h"
#include "Core/Object/Object.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/Technique.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/Scene/Base/Scene.h"

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

    std::shared_ptr<Object> Inspector::GetSelectObject() noexcept
    {
        return selectObject;
    }

    void Inspector::ShowNodeInfomation() noexcept
    {
        if (ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            if (selectObject)
            {
                ImGui::TextColored
                (
                    { 0.4f, 1.0f, 0.6f, 1.0f },
                    "Selected Object: %s",
                    selectObject->GetName().c_str()
                );

                // 기본 정보 표시
                ImGui::Separator();
                ImGui::Text("Transform Information");

                bool isNotMatch = false;

                // 위치 정보
                Position position = selectObject->transform->GetPosition();
                float pos[3] = { position.x, position.y, position.z };

                if (ImGui::DragFloat3("Position", pos, 0.1f))
                {
                    selectObject->transform->SetPosition(pos[0], pos[1], pos[2]);
                    isNotMatch = true;
                }

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
                    isNotMatch = true;
                }

                // 스케일 정보
                Scale scale = selectObject->transform->GetScale();
                float scl[3] = { scale.x, scale.y, scale.z };

                if (ImGui::DragFloat3("Scale", scl, 0.1f))
                {
                    selectObject->transform->SetScale(scl[0], scl[1], scl[2]);
                    isNotMatch = true;
                }

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

                // Model 컴포넌트가 있는지 확인하고 자동으로 Mesh 컴포넌트 표시
                if (auto modelComponent = selectObject->GetComponent("Model"))
                {
                    // Model 컴포넌트가 선택되어 있지 않더라도 Mesh 정보를 표시
                    if (selectComponent != modelComponent) // Model 컴포넌트가 이미 선택되어 있으면 중복 표시 방지
                    {
                        // 모델 계층 구조를 표시하고 메시를 컴포넌트처럼 보여주는 컨트롤러 클래스
                        class MeshComponentsController : public ModelBase
                        {
                        public:
                            bool push(SceneGraphNode& node) override
                            {
                                // 첫 번째 노드를 자동으로 선택
                                if (selectedNode == nullptr)
                                {
                                    selectedNode = &node;
                                    UpdateMeshComponents(node);
                                }

                                // 트리를 확장하지 않음 (간략하게 메시만 표시)
                                return false; 
                            }

                            void pop(SceneGraphNode& node) override {}

                            void UpdateMeshComponents(SceneGraphNode& node)
                            {
                                meshComponents.clear();
                                selectedMeshIndex = -1;

                                const auto& meshPtrs = node.GetMeshPtrs();
                                for (size_t i = 0; i < meshPtrs.size(); i++)
                                {
                                    MeshComponent comp;
                                    comp.mesh = meshPtrs[i];
                                    comp.name = "Mesh " + std::to_string(i);
                                    meshComponents.push_back(comp);
                                }
                            }

                            void ShowMeshComponents()
                            {
                                if (meshComponents.empty())
                                    return;

                                for (size_t i = 0; i < meshComponents.size(); i++)
                                {
                                    auto& comp = meshComponents[i];

                                    ImGui::PushID(static_cast<int>(i));

                                    // 메시 가시성 토글을 위한 체크박스
                                    bool isVisible = comp.mesh->GetVisible();
                                    if (ImGui::Checkbox("##MeshVisible", &isVisible))
                                        comp.mesh->SetVisible(isVisible);

                                    ImGui::SameLine();

                                    // 메시 이름 표시 (버튼으로 한다면 아래 주석 해제)
                                    ImGui::Text("%s", comp.name.c_str());

                                    // 속성 바로 표시
                                    ImGui::Indent();
                                    ImGui::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, "Properties:");
                                    ImGui::Indent();

                                    // 메시 가시성 설정
                                    ImGui::Checkbox("Visible", &isVisible);
                                    if (isVisible != comp.mesh->GetVisible())
                                        comp.mesh->SetVisible(isVisible);

                                    ImGui::Unindent();
                                    ImGui::Unindent();

                                    ImGui::PopID();
                                }
                            }

                        private:
                            struct MeshComponent
                            {
                                Mesh* mesh = nullptr;
                                std::string name;
                                bool isSelected = false;
                            };

                            SceneGraphNode* selectedNode = nullptr;     // 현재 선택된 노드
                            std::vector<MeshComponent> meshComponents;  // 메시 컴포넌트 목록
                            int selectedMeshIndex = -1;                 // 선택된 메시 컴포넌트 인덱스
                        };

                        static MeshComponentsController meshController;
                        auto modelObj = std::static_pointer_cast<Model>(modelComponent);

                        // 모델의 계층 구조에서 메시 추출
                        modelObj->Accept(meshController);

                        // 메시 컴포넌트 목록 표시
                        meshController.ShowMeshComponents();
                    }
                }

                else if (auto meshComponent = selectObject->GetComponent("MeshComponent"))
                {
                    // MeshComponent가 선택되어 있지 않더라도 Mesh 정보를 표시
                    if (selectComponent != meshComponent) // 이미 선택되어 있으면 중복 표시 방지
                    {
                        // MeshComponent에서 메시들을 표시하는 클래스
                        class MeshInfoController
                        {
                        public:
                            void UpdateMeshComponents(std::vector<Mesh*>& meshes)
                            {
                                meshComponents.clear();
                                selectedMeshIndex = -1;

                                for (size_t i = 0; i < meshes.size(); i++)
                                {
                                    MeshInfo comp;
                                    comp.mesh = meshes[i];
                                    comp.name = "Mesh " + std::to_string(i);
                                    meshComponents.push_back(comp);
                                }
                            }

                            void ShowMeshComponents()
                            {
                                if (meshComponents.empty())
                                    return;

                                for (size_t i = 0; i < meshComponents.size(); i++)
                                {
                                    auto& comp = meshComponents[i];

                                    ImGui::PushID(static_cast<int>(i));

                                    // 메시 가시성 토글을 위한 체크박스
                                    bool isVisible = comp.mesh->GetVisible();
                                    if (ImGui::Checkbox("##MeshVisible", &isVisible))
                                        comp.mesh->SetVisible(isVisible);

                                    ImGui::SameLine();

                                    // 메시 선택 버튼
                                    bool isSelected = (selectedMeshIndex == static_cast<int>(i));

                                    if (isSelected)
                                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));

                                    if (ImGui::Button(comp.name.c_str(), ImVec2(-1, 0)))
                                    {
                                        selectedMeshIndex = static_cast<int>(i);
                                        comp.isSelected = true;

                                        // 다른 메시는 선택 해제
                                        for (size_t j = 0; j < meshComponents.size(); j++)
                                        {
                                            if (j != i)
                                                meshComponents[j].isSelected = false;
                                        }
                                    }

                                    if (isSelected)
                                        ImGui::PopStyleColor();

                                    ImGui::PopID();
                                }

                                // 선택된 메시의 속성 표시
                                if (selectedMeshIndex >= 0 && selectedMeshIndex < meshComponents.size())
                                {
                                    ShowSelectedMeshProperties(meshComponents[selectedMeshIndex].mesh);
                                }
                            }

                            // 선택된 메시에 대한 속성 편집기 표시
                            void ShowSelectedMeshProperties(Mesh* mesh)
                            {
                                if (!mesh)
                                    return;

                                ImGui::Separator();
                                ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Mesh Properties");

                                // 메시 가시성 설정
                                bool isVisible = mesh->GetVisible();
                                if (ImGui::Checkbox("Visible", &isVisible))
                                    mesh->SetVisible(isVisible);

                                // TechniqueEditor를 사용하여 메시의 재질 속성 표시
                                TechniqueEditor editor;
                                mesh->Accept(editor);
                            }

                        private:
                            struct MeshInfo
                            {
                                Mesh* mesh = nullptr;
                                std::string name;
                                bool isSelected = false;
                            };

                            std::vector<MeshInfo> meshComponents;   // 메시 컴포넌트 목록
                            int selectedMeshIndex = -1;             // 선택된 메시 컴포넌트 인덱스
                        };

                        static MeshInfoController meshInfoController;

                        auto meshComp = std::static_pointer_cast<MeshComponent>(meshComponent);
                        auto& meshes = meshComp->GetMeshes();

                        meshInfoController.UpdateMeshComponents(meshes);
                    }
                }

                // 선택된 컴포넌트의 상세 정보 표시
                if (selectComponent)
                {
                    ImGui::Separator();
                    ImGui::TextColored
                    (
                        { 1.0f, 0.8f, 0.0f, 1.0f },
                        "Component: %s",
                        selectComponent->GetClassName().c_str()
                    );

                    // 컴포넌트 속성 표시 - 기본 속성만 표시
                    bool compIsActive = selectComponent->GetEnable();

                    if (ImGui::Checkbox("Component Active", &compIsActive))
                        selectComponent->SetEnable(compIsActive);

                    // TransformComponent인 경우 추가 정보 표시
                    if (selectComponent->GetClassName() == "TransformComponent")
                        ImGui::TextDisabled("Transform component is always required and cannot be removed.");

                    else if (auto colorObj = std::dynamic_pointer_cast<ColorObject>(selectComponent))
                        ColorObjectEditor();

                    else if (selectComponent->GetClassName() == "Model")
                        ModelEditor();

                    else if (selectComponent->GetClassName() == "MeshComponent")
                        MeshComponentEditor();

                    else if (selectComponent->GetClassName() == "PointLightComponent")
                    {
                        auto pointLightComponent = std::static_pointer_cast<PointLight>(selectComponent);
                        pointLightComponent->SpawnControlWidgets();
                    }

                    else if (selectComponent->GetClassName() == "CameraComponent")
                        CameraEditor();

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

    void Inspector::ColorObjectEditor() noexcept
    {
		class Probe : public TechniqueBase
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, technique->GetName().c_str());

                // Outline 테크닉은 체크박스를 표시하지 않음
                if (technique->GetName() != "Outline")
                {
                    bool isActive = technique->GetAcive();
                    ImGui::Checkbox(("Tech Active##"s + std::to_string(techIndex)).c_str(), &isActive);

                    technique->SetActive(isActive);
                }
			}

			bool OnVisitBuffer(DynamicConstantBuffer::Buffer& buffer) override
			{
				float isNotMatch = false;
				const auto IsNotMatch = [&isNotMatch](bool changed) { isNotMatch = isNotMatch || changed; };
				auto Tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufferIndex)](const char* label) mutable
				{
					tagScratch = label + tagString;

					return tagScratch.c_str();
				};

				if (auto data = buffer["materialColor"]; data.IsExist())
					IsNotMatch(ImGui::ColorEdit3(Tag("Material Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(data))));

				if (auto data = buffer["scale"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Scale"), &data, 1.0f, 2.0f, "%.3f"));

				if (auto data = buffer["color"]; data.IsExist())
					IsNotMatch(ImGui::ColorPicker4(Tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(data))));

				if (auto data = buffer["specularIntensity"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Specular Intensity"), &data, 0.0f, 1.0f));

				if (auto data = buffer["specularPower"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Glassiness"), &data, 0.0f, 1.0f, "%.1f"));

				return isNotMatch;
			}
		} probe;

		auto colorObject = std::static_pointer_cast<ColorObject>(selectComponent);
		colorObject->Accept(probe);
    }

    void Inspector::ModelEditor() noexcept
    {
        auto modelObject = std::static_pointer_cast<Model>(selectComponent);

        // 트리 구조 헤더
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Model Hierarchy");

        // 모델 계층 구조를 표시하고 메시를 컴포넌트처럼 보여주는 컨트롤러 클래스
        class ModelHierarchyController : public ModelBase
        {
        public:
            bool push(SceneGraphNode& node) override
            {
                // 노드 표시 (폴더처럼)
                const bool isExpanded = ImGui::TreeNodeEx
                (
                    (void*)(intptr_t)node.GetID(),
                    ImGuiTreeNodeFlags_OpenOnArrow |
                    ((selectedNode == &node) ? ImGuiTreeNodeFlags_Selected : 0) |
                    (node.hasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf),
                    "%s", node.GetName().c_str()
                );

                // 노드 클릭 처리
                if (ImGui::IsItemClicked())
                {
                    selectedNode = &node;

                    // 이 노드에 있는 메시들을 컴포넌트 리스트에 추가
                    UpdateMeshComponents(node);
                }

                return isExpanded;
            }

            void pop(SceneGraphNode& node) override
            {
                ImGui::TreePop();
            }

            // 노드의 메시들을 컴포넌트 리스트로 업데이트
            void UpdateMeshComponents(SceneGraphNode& node)
            {
                meshComponents.clear();
                selectedMeshIndex = -1;

                const auto& meshPtrs = node.GetMeshPtrs();
                for (size_t i = 0; i < meshPtrs.size(); i++)
                {
                    MeshComponent comp;
                    comp.mesh = meshPtrs[i];
                    comp.name = "Mesh " + std::to_string(i);
                    meshComponents.push_back(comp);
                }
            }

            // 메시 컴포넌트 목록 표시
            void ShowMeshComponents()
            {
                if (meshComponents.empty())
                    return;

                ImGui::Separator();
                ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Mesh Components");

                for (size_t i = 0; i < meshComponents.size(); i++)
                {
                    auto& comp = meshComponents[i];

                    ImGui::PushID(static_cast<int>(i));

                    // 메시 가시성 토글을 위한 체크박스
                    bool isVisible = comp.mesh->GetVisible();

                    if (ImGui::Checkbox("##MeshVisible", &isVisible))
                        comp.mesh->SetVisible(isVisible);

                    ImGui::SameLine();

                    // 컴포넌트 선택 버튼
                    bool isSelected = (selectedMeshIndex == static_cast<int>(i));

                    if (isSelected)
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));

                    if (ImGui::Button(comp.name.c_str(), ImVec2(-1, 0)))
                    {
                        selectedMeshIndex = static_cast<int>(i);
                        comp.isSelected = true;

                        // 다른 메시는 선택 해제
                        for (size_t j = 0; j < meshComponents.size(); j++)
                        {
                            if (j != i)
                                meshComponents[j].isSelected = false;
                        }
                    }

                    if (isSelected)
                        ImGui::PopStyleColor();

                    ImGui::PopID();
                }

                // 선택된 메시의 속성 표시
                if (selectedMeshIndex >= 0 && selectedMeshIndex < meshComponents.size())
                    ShowSelectedMeshProperties(meshComponents[selectedMeshIndex].mesh);
            }

            // 선택된 메시에 대한 속성 편집기 표시
            void ShowSelectedMeshProperties(Mesh* mesh)
            {
                if (!mesh)
                    return;

                ImGui::Separator();
                ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Mesh Properties");

                // 메시 가시성 설정
                bool isVisible = mesh->GetVisible();

                if (ImGui::Checkbox("Visible", &isVisible))
                    mesh->SetVisible(isVisible);

                // TechniqueEditor를 사용하여 메시의 재질 속성 표시
                TechniqueEditor editor;
                mesh->Accept(editor);
            }

        private:
            struct MeshComponent
            {
                Mesh* mesh = nullptr;
                std::string name;
                bool isSelected = false;
            };

            SceneGraphNode* selectedNode = nullptr;     // 현재 선택된 노드    
            std::vector<MeshComponent> meshComponents;  // 메시 컴포넌트 목록

            int selectedMeshIndex = -1;                 // 선택된 메시 컴포넌트 인덱스
        };

        // 모델 계층 구조 컨트롤러 생성
        static ModelHierarchyController controller;

        // 모델의 계층 구조 표시
        modelObject->Accept(controller);

        // 메시 컴포넌트 목록 표시
        controller.ShowMeshComponents();
    }

    void Inspector::MeshComponentEditor() noexcept
    {
        auto meshComponent = std::static_pointer_cast<MeshComponent>(selectComponent);

        // 메시 목록 표시
        ImGui::Separator();
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Meshes");

        auto& meshes = meshComponent->GetMeshes();

        // 메시 목록이 없으면 메시지 표시
        if (meshes.empty())
        {
            ImGui::TextDisabled("No meshes available");
            return;
        }

        // 각 메시 표시
        for (size_t i = 0; i < meshes.size(); i++)
        {
            auto mesh = meshes[i];

            ImGui::PushID(static_cast<int>(i));

            // 메시 속성 펼쳐서 보기
            if (ImGui::TreeNode(("Properties##" + std::to_string(i)).c_str()))
            {
                // 메시 가시성
                bool isVisible = mesh->GetVisible();

                if (ImGui::Checkbox("Visible##Detail", &isVisible))
                    mesh->SetVisible(isVisible);

                // 재질 속성 표시
                TechniqueEditor editor;
                mesh->Accept(editor);

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    void Inspector::CameraEditor() noexcept
    {
        // CameraContainer의 선택 UI를 먼저 표시
        ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "Camera Selection");

        // CameraContainer 인스턴스 가져오기
        auto scene = Scene::GetActiveScene();
        auto& cameraContainer = scene->GetCameraContainer();

        // CameraContainer의 Inspector용 UI 요소 표시
        cameraContainer.SpawnInspectorWidgets();

        ImGui::Separator();
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Camera Properties");

        // 카메라 컴포넌트의 고유 속성 표시
        auto cameraComponent = std::static_pointer_cast<Camera>(selectComponent);
        cameraComponent->SpawnControlWidgets();
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