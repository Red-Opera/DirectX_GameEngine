#pragma once

#include <filesystem>
#include <optional>
#include <type_traits>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/VSPS/ConstantBuffer.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include "Imgui/imgui.h"

// 오브젝트 일부분의 Mesh 부분을 역할하는 클래스
class Mesh : public Drawable
{
public:
	Mesh(DxGraphic& graphic, std::vector<std::shared_ptr<Graphic::Render>> bindPtr);

	void Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) NOEXCEPTRELEASE;
	
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;	// Mesh의 World 행렬을 반환함

private:
	mutable DirectX::XMFLOAT4X4 transform;	// Mesh의 World 행렬을 저장하는 행렬
};

// 오브젝트의 일부분을 나타내는 클래스 (자동차의 바퀴, 자동차의 몸체)
class SceneGraphNode
{
	friend class Model;

public:
	struct PSMaterialConstant
	{
		BOOL normalMapEnable = TRUE;
		BOOL hasSpecular = TRUE;
		BOOL hasGlassMap = FALSE;
		float specularPower = 3.1f;

		DirectX::XMFLOAT3 specularColor = { 0.75f, 0.75f, 0.75f };
		float specularMapWeight = 0.671f;
	};

	struct PSMaterialConstantNoTexture
	{
		DirectX::XMFLOAT4 materialColor = { 0.447970f, 0.327254f, 0.176283f, 1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f, 0.65f, 0.65f, 1.0f };
		float specularPower = 120.0f;
		float padding[3];
	};

	SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE;

	void Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;
	void ApplyWorldTranfsorm(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetTranform() const noexcept;
	void ShowTree(SceneGraphNode*& selectNode) const noexcept;
	int GetID() const noexcept;

	template<class T>
	bool MaterialControl(DxGraphic& graphic, T& constant)
	{
		if (meshPtrs.empty())
			return false;

		if constexpr (std::is_same<T, PSMaterialConstant>::value)
		{
			if (auto pcb = meshPtrs.front()->GetRenderObject<Graphic::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				bool normalMapEnabled = (bool)constant.normalMapEnable;
				ImGui::Checkbox("Norm Map", &normalMapEnabled);
				constant.normalMapEnable = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)constant.hasSpecular;
				ImGui::Checkbox("Spec Map", &specularMapEnabled);
				constant.hasSpecular = specularMapEnabled ? TRUE : FALSE;

				bool hasGlossMap = (bool)constant.hasGlassMap;
				ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
				constant.hasGlassMap = hasGlossMap ? TRUE : FALSE;

				ImGui::SliderFloat("Spec Weight", &constant.specularMapWeight, 0.0f, 2.0f);
				ImGui::SliderFloat("Spec Pow", &constant.specularPower, 0.0f, 1000.0f, "%f", 5.0f);
				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&constant.specularColor));
				pcb->Update(graphic, constant);
			}
		}

		else if constexpr (std::is_same<T, PSMaterialConstantNoTexture>::value)
		{
			if (auto pcb = meshPtrs.front()->GetRenderObject<Graphic::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				ImGui::SliderFloat("Spec Pow", &constant.specularPower, 0.0f, 1000.0f, "%f", 5.0f);
				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&constant.specularColor));
				ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&constant.materialColor));
				pcb->Update(graphic, constant);

				return true;
			}
		}

		return false;
	}

private:
	// 해당 오브젝트에 자식을 추가하는 메소드
	void AddChild(std::unique_ptr<SceneGraphNode> child) NOEXCEPTRELEASE;

	std::vector<std::unique_ptr<SceneGraphNode>> childPtrs;		// 자식 객체의 노드의 주소를 저장하는 배열
	std::vector<Mesh*> meshPtrs;								// 이 일부 오브젝트에 사용되는 Mesh를 저장하는 배열
	DirectX::XMFLOAT4X4 transform;								// 오브젝트의 Local Transform
	DirectX::XMFLOAT4X4 worldTransform;							// 오브젝트의 World Transform

	std::string name;	// 노드 이름
	int id;				// 노드 ID
};

// 전체적인 오브젝트를 나타내는 클래스
class Model
{
public:
	Model(DxGraphic& graphic, const std::string& pathString, float scale = 1.0f);

	void Draw(DxGraphic& graphic) const NOEXCEPTRELEASE;
	void ShowWindow(DxGraphic& graphic, const char* windowName = nullptr) noexcept;
	void SetRootTransform(DirectX::FXMMATRIX transform) noexcept;
	~Model() noexcept;

private:
	static std::unique_ptr<Mesh> ConvertMesh(DxGraphic& graphic, const aiMesh& mesh, const aiMaterial* const* materials, const std::filesystem::path& path, float scale);	// 모델에서 가져온 aiMesh를 Mesh 클래스로 변환
	std::unique_ptr<SceneGraphNode> ConvertSceneGraphNode(int& nextID, const aiNode& modelNode) NOEXCEPTRELEASE;			// 모델에서 가져온 aiNode를 SceneGraphNode로 변환하는 메소드

	std::unique_ptr<SceneGraphNode> root;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelHierarchy> modelHierarchy;	// 모델의 하이라이키 창
};

class ModelException : public BaseException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;

	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;

private:
	std::string note;
};