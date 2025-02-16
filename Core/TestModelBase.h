#pragma once

#include "Core/Draw/Mesh.h"
#include "Core/Draw/ModelBase.h"
#include "Core/Draw/SceneGraphNode.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

#include "Utility/StringConverter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class TB : public TechniqueBase
{
public:
	void OnSetTechnique() override
	{
		using namespace std::string_literals;
		ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, technique->GetName().c_str());

		bool isActive = technique->GetAcive();
		ImGui::Checkbox(("Tech Active##"s + std::to_string(techIndex)).c_str(), &isActive);

		technique->SetActive(isActive);
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
			IsNotMatch(ImGui::SliderFloat(Tag("Scale"), &data, 1.0f, 2.0f, "%.3f", 3.5f));

		if (auto data = buffer["offset"]; data.IsExist())
			IsNotMatch(ImGui::SliderFloat(Tag("Offset"), &data, 0.0f, 1.0f, "%.3f", 2.5f));

		if (auto data = buffer["materialColor"]; data.IsExist())
			IsNotMatch(ImGui::ColorPicker3(Tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(data))));

		if (auto data = buffer["specularColor"]; data.IsExist())
			IsNotMatch(ImGui::ColorPicker3(Tag("Specular Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(data))));

		if (auto data = buffer["specularGlass"]; data.IsExist())
			IsNotMatch(ImGui::SliderFloat(Tag("Specular Glass"), &data, 1.0f, 100.0f, "%.1f", 1.5f));

		if (auto data = buffer["specularIntensity"]; data.IsExist())
			IsNotMatch(ImGui::SliderFloat(Tag("Specular Intensity"), &data, 0.0f, 2.0f));

		if (auto data = buffer["useSpecularMap"]; data.IsExist())
			IsNotMatch(ImGui::Checkbox(Tag("Specular Map Enable"), &data));

		if (auto data = buffer["useNormalMap"]; data.IsExist())
			IsNotMatch(ImGui::Checkbox(Tag("Normal Map Enable"), &data));

		if (auto data = buffer["normalMapPower"]; data.IsExist())
			IsNotMatch(ImGui::SliderFloat(Tag("Normal Map Intensity"), &data, 0.0f, 2.0f));

		return isNotMatch;
	}
};

#undef CreateWindow
class MB : ModelBase
{
public:
	void CreateWindow(Model& model)
	{
		ImGui::Begin("Model");
		ImGui::Columns(2, nullptr, true);
		model.Accept(*this);
		ImGui::NextColumn();

		if (selectNode != nullptr)
		{
			bool isNotMath = false;
			const auto dcheck = [&isNotMath](bool changed) {isNotMath = isNotMath || changed; };
			auto& tf = GetTransform();

			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
			dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
			dcheck(ImGui::SliderAngle("X-rotation", &tf.roll, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Y-rotation", &tf.pitch, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Z-rotation", &tf.yaw, -180.0f, 180.0f));

			if (isNotMath)
			{
				selectNode->ApplyWorldTranfsorm(
					DirectX::XMMatrixRotationX(tf.roll) *
					DirectX::XMMatrixRotationY(tf.pitch) *
					DirectX::XMMatrixRotationZ(tf.yaw) *
					DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z)
				);
			}

			TB probe;
			selectNode->Accept(probe);
		}

		ImGui::End();
	}

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
	struct TransformType
	{
		float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
		float x = 0.0f, y = 0.0f, z = 0.0f;
	};

	TransformType& GetTransform() noexcept
	{
		const auto id = selectNode->GetID();
		auto i = transform.find(id);

		if (i == transform.end())
			return GetTranform(id);

		return i->second;
	}

	TransformType& GetTranform(int id) noexcept
	{
		const auto transform = selectNode->GetTranform();
		const auto angle = Vector::GetEulerAngle(transform);
		const auto position = Vector::GetPosition(transform);

		TransformType newTransform;
		newTransform.yaw = angle.z;
		newTransform.roll = angle.x;
		newTransform.pitch = angle.y;
		newTransform.x = position.x;
		newTransform.y = position.y;
		newTransform.z = position.z;

		return this->transform.insert({ id, {newTransform} }).first->second;
	}

	std::unordered_map<int, TransformType> transform;
	SceneGraphNode* selectNode = nullptr;
};