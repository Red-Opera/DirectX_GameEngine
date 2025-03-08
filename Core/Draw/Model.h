#pragma once

#include "Core/DxGraphic.h"

#include <string>
#include <memory>
#include <filesystem>

namespace RenderGraphNameSpace { class RenderGraph; }

class Mesh;
class ModelHierarchy;
class SceneGraphNode;

struct aiMesh;
struct aiMaterial;
struct aiNode;

// ��ü���� ������Ʈ�� ��Ÿ���� Ŭ����
class Model
{
public:
	Model(const std::string& pathString, float scale = 1.0f);

	void Submit(size_t size) const NOEXCEPTRELEASE;
	void Accept(class ModelBase& modelBase);

	void SetRootTransform(DirectX::FXMMATRIX transform) noexcept;
	void LinkTechniques(RenderGraphNameSpace::RenderGraph&);

	~Model() noexcept;

private:
	static std::unique_ptr<Mesh> ConvertMesh(const aiMesh& mesh, const aiMaterial* const* materials, const std::filesystem::path& path, float scale);	// �𵨿��� ������ aiMesh�� Mesh Ŭ������ ��ȯ
	std::unique_ptr<SceneGraphNode> ConvertSceneGraphNode(int& nextID, const aiNode& modelNode, float scale) noexcept;			// �𵨿��� ������ aiNode�� SceneGraphNode�� ��ȯ�ϴ� �޼ҵ�

	std::unique_ptr<SceneGraphNode> root;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};