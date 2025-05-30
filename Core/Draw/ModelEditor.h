#pragma once

#include "Core/Component/Transform/TransformComponent.h"
#include "Core/Draw/Mesh.h"
#include "Core/Draw/ModelBase.h"
#include "Core/Draw/SceneGraphNode.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

#include "Utility/StringConverter.h"

#include <External/Assimp/Importer.hpp>
#include <External/Assimp/scene.h>
#include <External/Assimp/postprocess.h>

#include <memory>

class TechniqueEditor : public TechniqueBase
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

		if (auto data = buffer["scale"]; data.IsExist())
			IsNotMatch(ImGui::DragFloat(Tag("Scale"), &data, 0.1f, 1.0f, 2.0f, "%.3f"));

		if (auto data = buffer["offset"]; data.IsExist())
			IsNotMatch(ImGui::DragFloat(Tag("Offset"), &data, 0.1f, 0.0f, 1.0f, "%.3f"));

		if (auto data = buffer["materialColor"]; data.IsExist())
			IsNotMatch(ImGui::ColorPicker3(Tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(data))));

		if (auto data = buffer["specularColor"]; data.IsExist())
			IsNotMatch(ImGui::ColorPicker3(Tag("Specular Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(data))));

		if (auto data = buffer["specularGlass"]; data.IsExist())
			IsNotMatch(ImGui::DragFloat(Tag("Specular Glass"), &data, 0.1f, 1.0f, 100.0f, "%.1f"));

		if (auto data = buffer["specularIntensity"]; data.IsExist())
			IsNotMatch(ImGui::DragFloat(Tag("Specular Intensity"), &data, 0.1f, 0.0f, 2.0f, "%.1f"));

		if (auto data = buffer["useSpecularMap"]; data.IsExist())
			IsNotMatch(ImGui::Checkbox(Tag("Specular Map Enable"), &data));

		if (auto data = buffer["useNormalMap"]; data.IsExist())
			IsNotMatch(ImGui::Checkbox(Tag("Normal Map Enable"), &data));

		if (auto data = buffer["normalMapPower"]; data.IsExist())
			IsNotMatch(ImGui::DragFloat(Tag("Normal Map Intensity"), &data, 0.1f , 0.0f, 2.0f, "%.1f"));

		return isNotMatch;
	}
};

#undef CreateWindow
class ModelEditor : ModelBase
{
public:
	ModelEditor(std::string name) : name(std::move(name)) { }

	void CreateWindow(class Model& model);

protected:
	bool push(SceneGraphNode& node) override
	{
		const int selectID = (selectNode == nullptr) ? -1 : selectNode->GetID();
		const auto nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow
			| ((node.GetID() == selectID) ? ImGuiTreeNodeFlags_Selected : 0)
			| (node.hasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);

		const auto expend = ImGui::TreeNodeEx((void*)(intptr_t)node.GetID(), nodeFlag, node.GetName().c_str());

		if (ImGui::IsItemClicked())
			selectNode = &node;

		return expend;
	}

	void pop(SceneGraphNode& node) override { ImGui::TreePop(); }

private:
	std::shared_ptr<TransformComponent> GetTransformComponent() noexcept
	{
		const auto id = selectNode->GetID();
		auto i = transform.find(id);

		if (i == transform.end())
			return SetTranformComponent(id);

		return i->second;
	}

	std::shared_ptr<TransformComponent> SetTranformComponent(int id) noexcept
	{
		auto transformComponent = selectNode->GetTransformComponent();

		return transform.insert({ id, {transformComponent} }).first->second;
	}

	std::unordered_map<int, std::shared_ptr<TransformComponent>> transform;
	SceneGraphNode* selectNode = nullptr;

	std::string name;
};