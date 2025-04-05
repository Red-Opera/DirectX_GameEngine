#pragma once

#include "Core/DxGraphic.h"

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

#include <memory>

class Mesh;
class TechniqueBase;
class ModelBase;

// 오브젝트의 일부분을 나타내는 클래스 (자동차의 바퀴, 자동차의 몸체)
class SceneGraphNode
{
	friend class Model;

public:
	SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE;

	void Submit(size_t channel, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;
	void Accept(ModelBase& modelBase);
	void Accept(TechniqueBase& techniqueBase);

	const Transform& GetTranform() const noexcept;
	const std::shared_ptr<TransformComponent>& GetTransformComponent() const noexcept;

	void ApplyWorldTransform(DirectX::FXMMATRIX transform) noexcept;
	bool hasChildren() const noexcept { return childPtrs.size() > 0; }

	int GetID() const noexcept;
	const std::string& GetName() const { return name; }

private:
	// 해당 오브젝트에 자식을 추가하는 메소드
	void AddChild(std::unique_ptr<SceneGraphNode> child) NOEXCEPTRELEASE;

	std::vector<std::unique_ptr<SceneGraphNode>> childPtrs;		// 자식 객체의 노드의 주소를 저장하는 배열
	std::vector<Mesh*> meshPtrs;								// 이 일부 오브젝트에 사용되는 Mesh를 저장하는 배열

	std::shared_ptr<Object> object;								// 이 노드가 속한 오브젝트
	std::shared_ptr<TransformComponent> transformComponent;		// 이 노드의 TransformComponent

	std::string name;	// 노드 이름
	int id;				// 노드 ID
};