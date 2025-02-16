#pragma once

#include "Core/DxGraphic.h"

#include <memory>

class Model;
class Mesh;
class TechniqueBase;
class ModelBase;

// ������Ʈ�� �Ϻκ��� ��Ÿ���� Ŭ���� (�ڵ����� ����, �ڵ����� ��ü)
class SceneGraphNode
{
	friend class Model;

public:
	SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE;

	void Submit(DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;
	void Accept(ModelBase& modelBase);
	void Accept(TechniqueBase& techniqueBase);

	void ApplyWorldTranfsorm(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetTranform() const noexcept;
	bool hasChildren() const noexcept { return childPtrs.size() > 0; }

	int GetID() const noexcept;
	const std::string& GetName() const { return name; }

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