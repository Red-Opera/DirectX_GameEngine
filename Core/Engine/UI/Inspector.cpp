癤#include "stdafx.h"
#include "Inspector.h"

#include "Core/Camera/Camera.h"
#include "Core/Camera/CameraContainer.h"
#include "Core/Component/MeshComponent.h"
#include "Core/Component/PhysicsComponent.h"
#include "Core/Component/Transform/TransformComponent.h"
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

                // 湲곕낯  
                ImGui::Separator();
                ImGui::Text("Transform Information");

                bool isNotMatch = false;

                // 移 ㅼ
                Position position = selectObject->transform->GetPosition();
                float pos[3] = { position.x, position.y, position.z };

                if (ImGui::DragFloat3("Position", pos, 0.1f))
                {
                    selectObject->transform->SetPosition(pos[0], pos[1], pos[2]);
                    isNotMatch = true;
                }

                // � ㅼ
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
                Scale scale = selectObject->transform->GetLocalScale();
                float scl[3] = { scale.x, scale.y, scale.z };

                if (ImGui::DragFloat3("Scale", scl, 0.1f))
                {
                    selectObject->transform->SetLocalScale(scl[0], scl[1], scl[2]);
                    isNotMatch = true;
                }

                // 而댄щ 紐⑸ 
                ImGui::Separator();
                ImGui::Text("Components");

                const auto& components = selectObject->GetAllComponents();

                for (const auto& component : components)
                {
                    ImGui::PushID(component.get());  // 怨 ID 遺

                    // 而댄щ 깊/鍮깊 泥댄щ
                    bool isActive = component->GetEnable();
                    if (ImGui::Checkbox("##ComponentActive", &isActive))
                        component->SetEnable(isActive);

                    ImGui::SameLine();

                    //  而댄щ 대쇱댄 
                    bool isSelected = (selectComponent == component);

                    if (isSelected)
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));

                    // 而댄щ 대 踰쇱쇰   媛ν寃 
                    if (ImGui::Button(component->GetClassName().c_str(), ImVec2(-1, 0)))
                    {
                        selectComponent = component;
                    }

                    if (isSelected)
                        ImGui::PopStyleColor();

                    ImGui::PopID();
                }

                // Model 而댄щ멸 吏 명怨 쇰 Mesh 而댄щ 
                if (auto modelComponent = selectObject->GetComponent("Model"))
                {
                    // Model 而댄щ멸  吏 ㅻ㈃ Mesh �蹂 
                    if (selectComponent != modelComponent) // Model 而댄щ멸 대�  ㅻ㈃ 以蹂  諛⑹
                    {
                        // 紐 몄 몃瑜 怨 硫щ� 而댄щ몄 愿由ы 而⑦몃· 대
                        class MeshComponentsController : public ModelBase
                        {
                        public:
                            bool push(SceneGraphNode& node) override
                            {
                                // 泥 踰吏 몃瑜 쇰 
                                if (selectedNode == nullptr)
                                {
                                    selectedNode = &node;
                                    UpdateMeshComponents(node);
                                }

                                // 몃━瑜 ν吏  (媛⑦寃 硫щ )
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

                                    // 硫 蹂댁닿린 瑜  泥댄щ
                                    bool isVisible = comp.mesh->GetVisible();
                                    if (ImGui::Checkbox("##MeshVisible", &isVisible))
                                        comp.mesh->SetVisible(isVisible);

                                    ImGui::SameLine();

                                    // 硫 대  (踰쇱쇰 ㅻ㈃  二쇱 댁)
                                    ImGui::Text("%s", comp.name.c_str());

                                    //  諛濡 
                                    ImGui::Indent();
                                    ImGui::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, "Properties:");
                                    ImGui::Indent();

                                    // 硫 蹂댁닿린 
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

                            SceneGraphNode* selectedNode = nullptr;     //   몃
                            std::vector<MeshComponent> meshComponents;  // 硫 而댄щ 紐⑸
                            int selectedMeshIndex = -1;                 //  硫 而댄щ 몃깆
                        };

                        static MeshComponentsController meshController;
                        auto modelObj = std::static_pointer_cast<Model>(modelComponent);

                        // 紐⑤ 대 援ъ“ 硫 吏
                        modelObj->Accept(meshController);

                        // 硫 而댄щ 紐⑸ 
                        meshController.ShowMeshComponents();
                    }
                }

                else if (auto meshComponent = selectObject->GetComponent("MeshComponent"))
                {
                    // MeshComponent媛  吏 ㅻ㈃ Mesh �蹂 
                    if (selectComponent != meshComponent) // 대�  ㅻ㈃ 以蹂  諛⑹
                    {
                        // MeshComponent 硫щㅼ  대
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

                                    // 硫 蹂댁닿린 瑜  泥댄щ
                                    bool isVisible = comp.mesh->GetVisible();
                                    if (ImGui::Checkbox("##MeshVisible", &isVisible))
                                        comp.mesh->SetVisible(isVisible);

                                    ImGui::SameLine();

                                    // 硫  踰
                                    bool isSelected = (selectedMeshIndex == static_cast<int>(i));

                                    if (isSelected)
                                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));

                                    if (ImGui::Button(comp.name.c_str(), ImVec2(-1, 0)))
                                    {
                                        selectedMeshIndex = static_cast<int>(i);
                                        comp.isSelected = true;

                                        // ㅻⅨ 硫щ  댁
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

                                //  硫ъ  
                                if (selectedMeshIndex >= 0 && selectedMeshIndex < meshComponents.size())
                                {
                                    ShowSelectedMeshProperties(meshComponents[selectedMeshIndex].mesh);
                                }
                            }

                            //  硫ъ   李쎌 
                            void ShowSelectedMeshProperties(Mesh* mesh)
                            {
                                if (!mesh)
                                    return;

                                ImGui::Separator();
                                ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Mesh Properties");

                                // 硫 蹂댁닿린 
                                bool isVisible = mesh->GetVisible();
                                if (ImGui::Checkbox("Visible", &isVisible))
                                    mesh->SetVisible(isVisible);

                                // TechniqueEditor瑜 ъ⑺ 硫ъ 湲고  
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

                            std::vector<MeshInfo> meshComponents;   // 硫 而댄щ 紐⑸
                            int selectedMeshIndex = -1;             //  硫 而댄щ 몃깆
                        };

                        static MeshInfoController meshInfoController;

                        auto meshComp = std::static_pointer_cast<MeshComponent>(meshComponent);
                        auto& meshes = meshComp->GetMeshes();

                        meshInfoController.UpdateMeshComponents(meshes);
                    }
                }

                //  而댄щ몄 몃 댁 
                if (selectComponent)
                {
                    ImGui::Separator();
                    ImGui::TextColored
                    (
                        { 1.0f, 0.8f, 0.0f, 1.0f },
                        "Component: %s",
                        selectComponent->GetClassName().c_str()
                    );

                    // 而댄щ   - 湲곕낯 깅 
                    bool compIsActive = selectComponent->GetEnable();

                    if (ImGui::Checkbox("Component Active", &compIsActive))
                        selectComponent->SetEnable(compIsActive);

                    // TransformComponent �  
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

                    else if (selectComponent->GetClassName() == "PhysicsComponent")
                        PhysicsComponentEditor();

                    // ㅻⅨ 而댄щ   뱀 UI瑜 ш린 異媛
                    else
                    {
                        if (ImGui::Button("Remove Component"))
                        {
                            selectObject->RemoveComponent(selectComponent->GetClassName());
                            selectComponent = nullptr;
                        }
                    }
                }

                // ㅻ� 湲곕낯 �蹂 
                ImGui::Separator();
                ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Object Details");

                // 깊   諛 蹂寃
                bool isActive = selectObject->GetActive();
                if (ImGui::Checkbox("Active", &isActive))
                    selectObject->SetActive(isActive);
                    
                // ㅻ� 대 몄
                char objectName[256];
                strcpy_s(objectName, 256, selectObject->GetName().c_str());

                if (ImGui::InputText("Name", objectName, 256))
                    selectObject->SetName(objectName);
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

                // Outline щ 泥댄щㅻ� 吏 
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

        // 몃━ 援ъ“ 
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Model Hierarchy");

        // 紐⑤몄 몃瑜 怨 硫щ� 而댄щ몄 愿由ы 而⑦몃· 대
        class ModelHierarchyController : public ModelBase
        {
        public:
            bool push(SceneGraphNode& node) override
            {
                // 몃  (몃━泥)
                const bool isExpanded = ImGui::TreeNodeEx
                (
                    (void*)(intptr_t)node.GetID(),
                    ImGuiTreeNodeFlags_OpenOnArrow |
                    ((selectedNode == &node) ? ImGuiTreeNodeFlags_Selected : 0) |
                    (node.hasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf),
                    "%s", node.GetName().c_str()
                );

                // 몃 대┃ 泥由
                if (ImGui::IsItemClicked())
                {
                    selectedNode = &node;

                    // 대 몃  硫щㅼ 而댄щ 由ъㅽ몄 異媛
                    UpdateMeshComponents(node);
                }

                return isExpanded;
            }

            void pop(SceneGraphNode& node) override
            {
                ImGui::TreePop();
            }

            //  硫щㅼ 而댄щ 由ъㅽ몃 곗댄
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

            // 硫 而댄щ 紐⑸ 
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

                    // 硫 蹂댁닿린 瑜  泥댄щ
                    bool isVisible = comp.mesh->GetVisible();

                    if (ImGui::Checkbox("##MeshVisible", &isVisible))
                        comp.mesh->SetVisible(isVisible);

                    ImGui::SameLine();

                    // 而댄щ  踰
                    bool isSelected = (selectedMeshIndex == static_cast<int>(i));

                    if (isSelected)
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));

                    if (ImGui::Button(comp.name.c_str(), ImVec2(-1, 0)))
                    {
                        selectedMeshIndex = static_cast<int>(i);
                        comp.isSelected = true;

                        // ㅻⅨ 硫щ  댁
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

                //  硫ъ  
                if (selectedMeshIndex >= 0 && selectedMeshIndex < meshComponents.size())
                    ShowSelectedMeshProperties(meshComponents[selectedMeshIndex].mesh);
            }

            //  硫ъ   李쎌 
            void ShowSelectedMeshProperties(Mesh* mesh)
            {
                if (!mesh)
                    return;

                ImGui::Separator();
                ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Mesh Properties");

                // 硫 蹂댁닿린 
                bool isVisible = mesh->GetVisible();

                if (ImGui::Checkbox("Visible", &isVisible))
                    mesh->SetVisible(isVisible);

                // TechniqueEditor瑜 ъ⑺ 硫ъ 湲고  
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

            SceneGraphNode* selectedNode = nullptr;     //   몃    
            std::vector<MeshComponent> meshComponents;  // 硫 而댄щ 紐⑸

            int selectedMeshIndex = -1;                 //  硫 而댄щ 몃깆
        };

        // 紐⑤ 怨痢 援ъ“ 而⑦몃· 
        static ModelHierarchyController controller;

        // 紐⑤ 怨痢 援ъ“ 
        modelObject->Accept(controller);

        // 硫 而댄щ 紐⑸ 
        controller.ShowMeshComponents();
    }

    void Inspector::MeshComponentEditor() noexcept
    {
        auto meshComponent = std::static_pointer_cast<MeshComponent>(selectComponent);

        // 硫 紐⑸ 
        ImGui::Separator();
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Meshes");

        auto& meshes = meshComponent->GetMeshes();

        // 硫 諛곗댁 鍮댁쇰㈃ 硫ш ㅺ� 
        if (meshes.empty())
        {
            ImGui::TextDisabled("No meshes available");
            return;
        }

        // 媛 硫 
        for (size_t i = 0; i < meshes.size(); i++)
        {
            auto mesh = meshes[i];

            ImGui::PushID(static_cast<int>(i));

            // 硫  �湲곗 硫
            if (ImGui::TreeNode(("Properties##" + std::to_string(i)).c_str()))
            {
                // 硫 蹂댁닿린
                bool isVisible = mesh->GetVisible();

                if (ImGui::Checkbox("Visible##Detail", &isVisible))
                    mesh->SetVisible(isVisible);

                // 湲고  
                TechniqueEditor editor;
                mesh->Accept(editor);

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    void Inspector::CameraEditor() noexcept
    {
        // CameraContainer � UI瑜 癒쇱 
        ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "Camera Selection");

        // CameraContainer 몄ㅽ댁 媛�몄ㅺ린
        auto scene = Scene::GetActiveScene();
        auto& cameraContainer = scene->GetCameraContainer();

        // CameraContainer媛 Inspector UI 李쎌 
        cameraContainer.SpawnInspectorWidgets();

        ImGui::Separator();
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Camera Properties");

        // 移대 而댄щ몄 媛蹂  
        auto cameraComponent = std::static_pointer_cast<Camera>(selectComponent);
        cameraComponent->SpawnControlWidgets();
    }

    void Inspector::PhysicsComponentEditor() noexcept
    {
        auto physicsComponent = std::static_pointer_cast<PhysicsComponent>(selectComponent);

        ImGui::Separator();
        ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Physics Properties");

        // 湲곕낯 臾쇰━ ㅼ
        if (ImGui::CollapsingHeader("Basic Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 대� 媛泥(Kinematic) ㅼ
            bool isKinematic = physicsComponent->IsKinematic();

            if (ImGui::Checkbox("Is Kinematic", &isKinematic))
            {
                physicsComponent->SetKinematic(isKinematic);
                
                // ㅻㅻ ㅼ  댄 
                if (isKinematic && ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Kinematic objects are controlled manually");
                    ImGui::Text("and not affected by physics simulation");
                    ImGui::EndTooltip();
                }
            }
            
            // ㅻㅻ깆  留 以� 諛 吏 ㅼ 
            if (!isKinematic)
            {
                // 以� ъ щ
                bool useGravity = physicsComponent->IsGravityEnabled();
                if (ImGui::Checkbox("Use Gravity", &useGravity))
                    physicsComponent->SetGravity(useGravity);
                
                // 以� ъ  以� ㅼ ㅼ
                if (useGravity)
                {
                    float gravityScale = physicsComponent->GetGravityScale();
                    if (ImGui::SliderFloat("Gravity Scale", &gravityScale, 0.0f, 5.0f, "%.2f"))
                        physicsComponent->SetGravityScale(gravityScale);
                    
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("Multiplier for global gravity");
                        ImGui::Text("Default: 1.0 (normal gravity)");
                        ImGui::EndTooltip();
                    }
                }

                // 吏 ㅼ
                float mass = physicsComponent->GetMass();

                if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.1f, 1000.0f))
                    physicsComponent->SetMass(mass);
            }
            
            // 異⑸ 媛吏 紐⑤ 
            static const char* collisionModes[] = { "Discrete", "Continuous", "Continuous Dynamic" };
            int currentMode = static_cast<int>(physicsComponent->GetCollisionDetectionMode());
            
            if (ImGui::Combo("Collision Detection", &currentMode, collisionModes, IM_ARRAYSIZE(collisionModes)))
                physicsComponent->SetCollisionDetectionMode(static_cast<CollisionDetectionMode>(currentMode));
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Discrete: Basic collision detection (default)");
                ImGui::TextUnformatted("Continuous: For fast-moving objects");
                ImGui::TextUnformatted("Continuous Dynamic: Best for fast object collisions");
                ImGui::EndTooltip();
            }
        }

        // � 以� ㅼ
        if (ImGui::CollapsingHeader("Global Gravity"))
        {
            physx::PxVec3 gravity = PhysicsComponent::GetGlobalGravity();
            float gravityArray[3] = { gravity.x, gravity.y, gravity.z };
            
            if (ImGui::DragFloat3("Gravity Force", gravityArray, 0.1f, -50.0f, 50.0f))
                PhysicsComponent::SetGlobalGravity(gravityArray[0], gravityArray[1], gravityArray[2]);
            
            if (ImGui::Button("Reset to Default", ImVec2(120, 0)))
                PhysicsComponent::SetGlobalGravity(0.0f, -9.81f, 0.0f);
            
            ImGui::SameLine();
            ImGui::TextDisabled("Default: (0, -9.81, 0)");
        }

        // 臾쇱 
        if (ImGui::CollapsingHeader("Material Properties"))
        {
            float staticFriction = physicsComponent->GetStaticFriction();
            float dynamicFriction = physicsComponent->GetDynamicFriction();
            float restitution = physicsComponent->GetRestitution();
            
            bool materialChanged = false;
            
            if (ImGui::SliderFloat("Static Friction", &staticFriction, 0.0f, 2.0f))
                materialChanged = true;
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Friction when object is not moving");
                ImGui::Text("0 = No friction, 1 = Normal, 2 = High");
                ImGui::EndTooltip();
            }
            
            if (ImGui::SliderFloat("Dynamic Friction", &dynamicFriction, 0.0f, 2.0f))
                materialChanged = true;
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Friction when object is moving");
                ImGui::Text("0 = No friction, 1 = Normal, 2 = High");
                ImGui::EndTooltip();
            }
            
            if (ImGui::SliderFloat("Bounciness", &restitution, 0.0f, 1.0f))
                materialChanged = true;
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("How bouncy the object is");
                ImGui::Text("0 = No bounce, 1 = Perfect bounce");
                ImGui::EndTooltip();
            }
            
            if (materialChanged)
                physicsComponent->SetMaterial(staticFriction, dynamicFriction, restitution);
            
            // 誘몃━ � 由ъ 踰쇰
            ImGui::Separator();
            ImGui::TextColored({ 0.7f, 0.7f, 1.0f, 1.0f }, "Material Presets:");
            
            if (ImGui::Button("Ice", ImVec2(60, 0)))
                physicsComponent->SetMaterial(0.05f, 0.02f, 0.2f);
                
            ImGui::SameLine();
            
            if (ImGui::Button("Metal", ImVec2(60, 0)))
                physicsComponent->SetMaterial(0.6f, 0.4f, 0.5f);
                
            ImGui::SameLine();
            
            if (ImGui::Button("Wood", ImVec2(60, 0)))
                physicsComponent->SetMaterial(0.5f, 0.3f, 0.2f);
                
            ImGui::SameLine();
            
            if (ImGui::Button("Rubber", ImVec2(60, 0)))
                physicsComponent->SetMaterial(0.8f, 0.7f, 0.8f);
                
            ImGui::SameLine();
            
            if (ImGui::Button("Bouncy", ImVec2(60, 0)))
                physicsComponent->SetMaterial(0.2f, 0.2f, 0.95f);
        }

        // 怨듦린 � 諛 媛
        if (ImGui::CollapsingHeader("Damping & Drag"))
        {
            //  媛 (대  媛)
            float linearDamping = physicsComponent->GetLinearDamping();
            if (ImGui::SliderFloat("Linear Damping", &linearDamping, 0.0f, 10.0f))
                physicsComponent->SetLinearDamping(linearDamping);
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Reduces linear velocity over time");
                ImGui::Text("0 = No damping, 10 = Very high damping");
                ImGui::EndTooltip();
            }
            
            // 媛 媛 (�  媛)
            float angularDamping = physicsComponent->GetAngularDamping();
            if (ImGui::SliderFloat("Angular Damping", &angularDamping, 0.0f, 10.0f))
                physicsComponent->SetAngularDamping(angularDamping);
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Reduces rotational velocity over time");
                ImGui::Text("0 = No damping, 10 = Very high damping");
                ImGui::EndTooltip();
            }
            
            // 怨듦린 � 怨 (異媛 媛)
            float dragCoefficient = physicsComponent->GetDragCoefficient();
            if (ImGui::SliderFloat("Drag Coefficient", &dragCoefficient, 0.0f, 5.0f, "%.2f"))
                physicsComponent->SetDragCoefficient(dragCoefficient);
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Additional drag force (air/fluid resistance)");
                ImGui::Text("0 = None, 1 = Air, 2-5 = Water/thick fluid");
                ImGui::EndTooltip();
            }
            
            // 由ъ 踰쇰
            ImGui::Separator();
            ImGui::TextColored({ 0.7f, 0.7f, 1.0f, 1.0f }, "Environment Presets:");
            
            if (ImGui::Button("Air", ImVec2(60, 0)))
            {
                physicsComponent->SetLinearDamping(0.05f);
                physicsComponent->SetAngularDamping(0.1f);
                physicsComponent->SetDragCoefficient(0.0f);
            }
                
            ImGui::SameLine();
            
            if (ImGui::Button("Water", ImVec2(60, 0)))
            {
                physicsComponent->SetLinearDamping(0.5f);
                physicsComponent->SetAngularDamping(1.0f);
                physicsComponent->SetDragCoefficient(2.0f);
            }
                
            ImGui::SameLine();
            
            if (ImGui::Button("Honey", ImVec2(60, 0)))
            {
                physicsComponent->SetLinearDamping(2.0f);
                physicsComponent->SetAngularDamping(4.0f);
                physicsComponent->SetDragCoefficient(4.0f);
            }
                
            ImGui::SameLine();
            
            if (ImGui::Button("Space", ImVec2(60, 0)))
            {
                physicsComponent->SetLinearDamping(0.0f);
                physicsComponent->SetAngularDamping(0.0f);
                physicsComponent->SetDragCoefficient(0.0f);
            }
        }

        // � 議곌굔
        if (ImGui::CollapsingHeader("Constraints"))
        {
            ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Freeze Position");

            bool freezePosX = physicsComponent->IsPositionXFrozen();
            bool freezePosY = physicsComponent->IsPositionYFrozen();
            bool freezePosZ = physicsComponent->IsPositionZFrozen();

            bool positionChanged = false;

            if (ImGui::Checkbox("X##PosX", &freezePosX))
                positionChanged = true;

            ImGui::SameLine();

            if (ImGui::Checkbox("Y##PosY", &freezePosY))
                positionChanged = true;

            ImGui::SameLine();

            if (ImGui::Checkbox("Z##PosZ", &freezePosZ))
                positionChanged = true;

            if (positionChanged)
                physicsComponent->SetFreezePosition(freezePosX, freezePosY, freezePosZ);

            ImGui::Spacing();
            ImGui::TextColored({ 1.0f, 0.8f, 0.0f, 1.0f }, "Freeze Rotation");

            bool freezeRotX = physicsComponent->IsRotationXFrozen();
            bool freezeRotY = physicsComponent->IsRotationYFrozen();
            bool freezeRotZ = physicsComponent->IsRotationZFrozen();

            bool rotationChanged = false;

            if (ImGui::Checkbox("X##RotX", &freezeRotX))
                rotationChanged = true;

            ImGui::SameLine();

            if (ImGui::Checkbox("Y##RotY", &freezeRotY))
                rotationChanged = true;

            ImGui::SameLine();

            if (ImGui::Checkbox("Z##RotZ", &freezeRotZ))
                rotationChanged = true;

            if (rotationChanged)
                physicsComponent->SetFreezeRotation(freezeRotX, freezeRotY, freezeRotZ);

            // 異蹂 � 議곌굔 踰쇰
            ImGui::Separator();
            ImGui::TextColored({ 0.7f, 0.7f, 1.0f, 1.0f }, "Common Constraints:");

            if (ImGui::Button("Lock All", ImVec2(80, 0)))
            {
                physicsComponent->SetFreezePosition(true, true, true);
                physicsComponent->SetFreezeRotation(true, true, true);
            }

            ImGui::SameLine();

            if (ImGui::Button("Unlock All", ImVec2(80, 0)))
            {
                physicsComponent->SetFreezePosition(false, false, false);
                physicsComponent->SetFreezeRotation(false, false, false);

                // ъ 吏 
                auto scene = Scene::GetActiveScene();

                if (scene == nullptr)
                {
                    // 紐�쇰 源⑥곌린
                    if (auto actor = physicsComponent->GetActor())
                    {
                        if (physicsComponent->IsKinematic() == false)
                        {
                            physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
                            dynamicActor->wakeUp();

                            // 以�μ   쎄  媛湲
                            if (!physicsComponent->IsGravityEnabled())
                                dynamicActor->addForce(physx::PxVec3(0.0f, 0.1f, 0.0f), physx::PxForceMode::eIMPULSE);
                        }
                    }
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("2D (XZ)", ImVec2(80, 0)))
            {
                physicsComponent->SetFreezePosition(false, true, false);
                physicsComponent->SetFreezeRotation(true, false, true);
            }
        }

        // 肄쇱대 ㅼ
        if (ImGui::CollapsingHeader("Collider"))
        {
            ImGui::TextColored({ 0.7f, 0.7f, 1.0f, 1.0f }, "Collider Size:");
            
            // 異⑸泥 ш린 곗댄 踰
            if (ImGui::Button("Update Collider Size"))
                physicsComponent->UpdateColliderSize();
            
            ImGui::SameLine();
            
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Updates collider to match current object scale");
                ImGui::EndTooltip();
            }
        }

        // 而댄щ � 踰
        ImGui::Separator();

        if (ImGui::Button("Remove Physics Component", ImVec2(-1, 0)))
        {
            selectObject->RemoveComponent(selectComponent->GetClassName());
            selectComponent = nullptr;
        }
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