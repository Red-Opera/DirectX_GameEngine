#pragma once

#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Vertex.h"
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
	SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE;

	void Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;
	void ApplyWorldTranfsorm(DirectX::FXMMATRIX transform) noexcept;
	void ShowTree(SceneGraphNode*& selectNode) const noexcept;
	int GetID() const noexcept;

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
	Model(DxGraphic& graphic, const std::string fileName);

	void Draw(DxGraphic& graphic) const NOEXCEPTRELEASE;
	void ShowWindow(const char* windowName = nullptr) noexcept;
	~Model() noexcept;

private:
	static std::unique_ptr<Mesh> ConvertMesh(DxGraphic& graphic, const aiMesh& mesh, const aiMaterial* const* materials);	// 모델에서 가져온 aiMesh를 Mesh 클래스로 변환
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