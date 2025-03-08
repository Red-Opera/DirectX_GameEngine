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

// 전체적인 오브젝트를 나타내는 클래스
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
	static std::unique_ptr<Mesh> ConvertMesh(const aiMesh& mesh, const aiMaterial* const* materials, const std::filesystem::path& path, float scale);	// 모델에서 가져온 aiMesh를 Mesh 클래스로 변환
	std::unique_ptr<SceneGraphNode> ConvertSceneGraphNode(int& nextID, const aiNode& modelNode, float scale) noexcept;			// 모델에서 가져온 aiNode를 SceneGraphNode로 변환하는 메소드

	std::unique_ptr<SceneGraphNode> root;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};