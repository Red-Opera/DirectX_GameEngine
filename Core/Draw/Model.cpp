#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "SceneGraphNode.h"

#include "Base/Material.h"
#include "Core/Exception/ModelException.h"
#include "Utility/MathInfo.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(DxGraphic& graphic, const std::string& pathString, const float scale)
{
	Assimp::Importer importer;

	const auto model = importer.ReadFile(pathString.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (model == nullptr)
		throw ModelException(__LINE__, __FILE__, importer.GetErrorString());

	std::vector<Material> materials;
	materials.reserve(model->mNumMaterials);

	for (size_t i = 0; i < model->mNumMaterials; i++)
		materials.emplace_back(graphic, *model->mMaterials[i], pathString);

	for (size_t i = 0; i < model->mNumMeshes; i++)
	{
		const auto& mesh = *model->mMeshes[i];
		meshPtrs.push_back(std::make_unique<Mesh>(graphic, materials[mesh.mMaterialIndex], mesh, scale));
	}

	int nextID = 0;
	root = ConvertSceneGraphNode(nextID, *model->mRootNode, scale);
}

void Model::Submit(size_t channel) const NOEXCEPTRELEASE
{
	//modelHierarchy->ApplyParameter();
	root->Submit(channel, DirectX::XMMatrixIdentity());
}

void Model::Accept(ModelBase& modelBase)
{
	root->Accept(modelBase);
}

void Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
{
	root->ApplyWorldTranfsorm(transform);
}

void Model::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	for (auto& mesh : meshPtrs)
		mesh->LinkTechniques(renderGraph);
}

Model::~Model() noexcept
{
}

std::unique_ptr<SceneGraphNode> Model::ConvertSceneGraphNode(int& nextID, const aiNode& modelNode, float scale) noexcept
{
	const auto transform = 
		Math::MultipleMatrixScale
		(
			DirectX::XMMatrixTranspose
			(
				DirectX::XMLoadFloat4x4
				(
					reinterpret_cast<const DirectX::XMFLOAT4X4*>(&modelNode.mTransformation)
				)
			)
			, scale
		);

	std::vector<Mesh*> currentMeshPtrs;
	currentMeshPtrs.reserve(modelNode.mNumMeshes);

	for (size_t i = 0; i < modelNode.mNumMeshes; i++)
	{
		const auto meshIndex = modelNode.mMeshes[i];
		currentMeshPtrs.push_back(meshPtrs.at(meshIndex).get());
	}

	auto node = std::make_unique<SceneGraphNode>(nextID++, modelNode.mName.C_Str(), std::move(currentMeshPtrs), transform);

	for (size_t i = 0; i < modelNode.mNumChildren; i++)
		node->AddChild(ConvertSceneGraphNode(nextID, *modelNode.mChildren[i], scale));

	return node;
}