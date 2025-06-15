#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "SceneGraphNode.h"

#include "Base/Material.h"
#include "Core/App.h"
#include "Core/Component/MeshComponent.h"
#include "Core/Exception/ModelException.h"
#include "Core/Object/Object.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Utility/MathInfo.h"

#include <External/Assimp/Importer.hpp>
#include <External/Assimp/scene.h>
#include <External/Assimp/postprocess.h>

Model::Model(std::shared_ptr<class Object> object, const std::string& pathString, const float scale)
    : Component(object), modelScale(scale)
{
    Assimp::Importer importer;

    const auto model = importer.ReadFile(pathString.c_str(),
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (model == nullptr)
        throw ModelException(__LINE__, __FILE__, importer.GetErrorString());

    std::vector<Material> materials;
    materials.reserve(model->mNumMaterials);

    for (size_t i = 0; i < model->mNumMaterials; i++)
        materials.emplace_back(*model->mMaterials[i], pathString);

    for (size_t i = 0; i < model->mNumMeshes; i++)
    {
        const auto& mesh = *model->mMeshes[i];
        meshPtrs.push_back(std::make_unique<Mesh>(materials[mesh.mMaterialIndex], mesh, scale));
    }

    // PhysX 콜라이더용 정점 데이터 미리 캐싱
    meshVertices.resize(model->mNumMeshes);
    meshIndices.resize(model->mNumMeshes);
    
    for (size_t i = 0; i < model->mNumMeshes; i++)
    {
        const auto& mesh = *model->mMeshes[i];
        const auto& material = materials[mesh.mMaterialIndex];
        
        // 정점 데이터 추출
        auto vertexBuffer = material.GetVertex(mesh);
        auto indexData = material.GetIndex(mesh);
        
        // PhysX 형식으로 변환
        for (size_t v = 0; v < vertexBuffer.count(); v++)
        {
            auto pos = vertexBuffer[v].GetValue<VertexCore::VertexLayout::VertexType::Position3D>();
            meshVertices[i].emplace_back(pos.x * scale, pos.y * scale, pos.z * scale);
        }
        
        // 인덱스 데이터 변환
        for (auto index : indexData)
            meshIndices[i].push_back(static_cast<uint32_t>(index));
    }
    
    physxDataCached = true;

    int nextID = 0;
    root = ConvertSceneGraphNode(nextID, *model->mRootNode, scale, object);
}

bool Model::GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
    std::vector<uint32_t>& indices,
    const Scale& scale) const
{
    vertices.clear();
    indices.clear();
    
    if (!physxDataCached || meshVertices.empty())
        return false;
        
    try
    {
        uint32_t vertexOffset = 0;
        
        // 모든 메시의 정점과 인덱스를 결합
        for (size_t meshIdx = 0; meshIdx < meshVertices.size(); meshIdx++)
        {
            // 정점 데이터 추가 (추가 스케일 적용)
            for (const auto& vertex : meshVertices[meshIdx])
            {
                vertices.emplace_back(
                    vertex.x * scale.x,
                    vertex.y * scale.y,
                    vertex.z * scale.z
                );
            }
            
            // 인덱스 데이터 추가 (오프셋 적용)
            for (const auto& index : meshIndices[meshIdx])
                indices.push_back(index + vertexOffset);
            
            vertexOffset += static_cast<uint32_t>(meshVertices[meshIdx].size());
        }
        
        return !vertices.empty() && !indices.empty();
    }

    catch (...)
    {
        vertices.clear();
        indices.clear();
        return false;
    }
}

void Model::Submit(size_t channel) const NOEXCEPTRELEASE
{
	root->Submit(channel, DirectX::XMMatrixIdentity());
}

void Model::Accept(ModelBase& modelBase)
{
	root->Accept(modelBase);
}

void Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
{
	root->ApplyWorldTransform(transform);
}

void Model::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	for (auto& mesh : meshPtrs)
		mesh->LinkTechniques(renderGraph);
}

void Model::Initialize()
{
	Component::Initialize();

	if (transform && root)
		root->transformComponent = transform;

	LinkTechniques(App::GetRenderGraph());
}

void Model::Update(float deltaTime)
{
	Component::Update(deltaTime);

	Submit(RenderingChannel::main);
	Submit(RenderingChannel::shadow);
}

void Model::LateUpdate()
{
	Component::LateUpdate();
}

Model::~Model() noexcept
{

}

std::unique_ptr<SceneGraphNode> Model::ConvertSceneGraphNode(int& nextID, const aiNode& modelNode, float scale, std::shared_ptr<class Object> root) noexcept
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

	std::unique_ptr<SceneGraphNode> node;
	
	if (nextID == 0)
		node = std::make_unique<SceneGraphNode>(nextID++, modelNode.mName.C_Str(), std::move(currentMeshPtrs), transform, root);

	else
		node = std::make_unique<SceneGraphNode>(nextID++, modelNode.mName.C_Str(), std::move(currentMeshPtrs), transform);

	for (size_t i = 0; i < modelNode.mNumChildren; i++)
		node->AddChild(ConvertSceneGraphNode(nextID, *modelNode.mChildren[i], scale));

	return node;
}