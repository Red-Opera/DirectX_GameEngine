#pragma once

#include "Core/Component/Component.h"
#include "Core/DxGraphic.h"
#include "Core/Draw/ModelBase.h"

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
class Model : public Component
{
public:
	Model(std::shared_ptr<class Object> object, const std::string& pathString, float scale = 1.0f);

	void Submit(size_t channel) const NOEXCEPTRELEASE;
	void Accept(class ModelBase& modelBase);

	void SetRootTransform(DirectX::FXMMATRIX transform) noexcept;
	void LinkTechniques(RenderGraphNameSpace::RenderGraph&);

	~Model() noexcept;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;

	virtual std::string GetClassName() const override { return "Model"; }
	static std::string GetStaticClassName() { return "Model"; }

private:
	std::unique_ptr<SceneGraphNode> ConvertSceneGraphNode
	(
		int& nextID, 
		const aiNode& modelNode, 
		float scale, 
		std::shared_ptr<class Object> root = nullptr
	) noexcept;			// 모델에서 가져온 aiNode를 SceneGraphNode로 변환하는 메소드

	std::unique_ptr<SceneGraphNode> root;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};