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

// ������Ʈ �Ϻκ��� Mesh �κ��� �����ϴ� Ŭ����
class Mesh : public Drawable
{
public:
	Mesh(DxGraphic& graphic, std::vector<std::shared_ptr<Graphic::Render>> bindPtr);

	void Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) NOEXCEPTRELEASE;
	
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;	// Mesh�� World ����� ��ȯ��

private:
	mutable DirectX::XMFLOAT4X4 transform;	// Mesh�� World ����� �����ϴ� ���
};

// ������Ʈ�� �Ϻκ��� ��Ÿ���� Ŭ���� (�ڵ����� ����, �ڵ����� ��ü)
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
	// �ش� ������Ʈ�� �ڽ��� �߰��ϴ� �޼ҵ�
	void AddChild(std::unique_ptr<SceneGraphNode> child) NOEXCEPTRELEASE;

	std::vector<std::unique_ptr<SceneGraphNode>> childPtrs;		// �ڽ� ��ü�� ����� �ּҸ� �����ϴ� �迭
	std::vector<Mesh*> meshPtrs;								// �� �Ϻ� ������Ʈ�� ���Ǵ� Mesh�� �����ϴ� �迭
	DirectX::XMFLOAT4X4 transform;								// ������Ʈ�� Local Transform
	DirectX::XMFLOAT4X4 worldTransform;							// ������Ʈ�� World Transform

	std::string name;	// ��� �̸�
	int id;				// ��� ID
};

// ��ü���� ������Ʈ�� ��Ÿ���� Ŭ����
class Model
{
public:
	Model(DxGraphic& graphic, const std::string& pathString, float scale = 1.0f);

	void Draw(DxGraphic& graphic) const NOEXCEPTRELEASE;
	void ShowWindow(DxGraphic& graphic, const char* windowName = nullptr) noexcept;
	void SetRootTransform(DirectX::FXMMATRIX transform) noexcept;
	~Model() noexcept;

private:
	static std::unique_ptr<Mesh> ConvertMesh(DxGraphic& graphic, const aiMesh& mesh, const aiMaterial* const* materials, const std::filesystem::path& path, float scale);	// �𵨿��� ������ aiMesh�� Mesh Ŭ������ ��ȯ
	std::unique_ptr<SceneGraphNode> ConvertSceneGraphNode(int& nextID, const aiNode& modelNode) NOEXCEPTRELEASE;			// �𵨿��� ������ aiNode�� SceneGraphNode�� ��ȯ�ϴ� �޼ҵ�

	std::unique_ptr<SceneGraphNode> root;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelHierarchy> modelHierarchy;	// ���� ���̶���Ű â
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