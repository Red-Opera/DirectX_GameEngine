#pragma once

#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Vertex.h"
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
	SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE;

	void Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;
	void ApplyWorldTranfsorm(DirectX::FXMMATRIX transform) noexcept;
	void ShowTree(SceneGraphNode*& selectNode) const noexcept;
	int GetID() const noexcept;

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
	Model(DxGraphic& graphic, const std::string fileName);

	void Draw(DxGraphic& graphic) const NOEXCEPTRELEASE;
	void ShowWindow(const char* windowName = nullptr) noexcept;
	~Model() noexcept;

private:
	static std::unique_ptr<Mesh> ConvertMesh(DxGraphic& graphic, const aiMesh& mesh, const aiMaterial* const* materials);	// �𵨿��� ������ aiMesh�� Mesh Ŭ������ ��ȯ
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