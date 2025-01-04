#include "stdafx.h"
#include "Mesh.h"

#include "Base/Material.h"

#include <filesystem>
#include <unordered_map>
#include <sstream>

class ModelHierarchy
{
public:
	void ShowWindow(DxGraphic& graphic, const char* windowName, const SceneGraphNode& root) noexcept
	{
		windowName = windowName ? windowName : "Model";

		int nodeIndex = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(selectNode);

			ImGui::NextColumn();

			if (selectNode != nullptr)
			{
				const auto id = selectNode->GetID();
				auto i = transforms.find(id);

				if (i == transforms.end())
				{
					const auto& nodeTransform = selectNode->GetTranform();
					const auto angle = Vector::GetEulerAngle(nodeTransform);
					const auto position = Vector::GetPosition(nodeTransform);

					TransformType getNodeTransform;
					getNodeTransform.roll = angle.z;
					getNodeTransform.pitch = angle.x;
					getNodeTransform.yaw = angle.y;
					getNodeTransform.x = position.x;
					getNodeTransform.y = position.y;
					getNodeTransform.z = position.z;

					std::tie(i, std::ignore) = transforms.insert({ id, getNodeTransform });
				}

				auto& transform = i->second;
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

				if (!selectNode->MaterialControl(graphic, psConstant))
					selectNode->MaterialControl(graphic, ringMaterial);
			}
		}
		ImGui::End();
	}

	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert(selectNode != nullptr && "선택된 노드가 존재하지 않습니다");

		const auto& transform = transforms.at(selectNode->GetID());

		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	SceneGraphNode* GetSelectNode() const noexcept
	{
		return selectNode;
	}

private:
	struct TransformType
	{
		float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
		float x = 0.0f, y = 0.0f, z = 0.0f;
	};

	SceneGraphNode* selectNode;								// 선택한 모델의 부분 노드
	SceneGraphNode::PSMaterialConstant psConstant;
	SceneGraphNode::PSMaterialConstantNoTexture ringMaterial;

	std::unordered_map<int, TransformType> transforms;
};

Mesh::Mesh(DxGraphic& graphic, std::vector<std::shared_ptr<Graphic::Render>> bindPtr)
{
	AddRender(Graphic::PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& renderingState : bindPtr)
		AddRender(std::move(renderingState));

	AddRender(std::make_shared<Graphic::TransformConstantBuffer>(graphic, *this));
}

void Mesh::Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) NOEXCEPTRELEASE
{
	DirectX::XMStoreFloat4x4(&transform, parentWorldTransform);
	Drawable::Draw(graphic);
}

DirectX::XMMATRIX Mesh::GetTransformMatrix() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

SceneGraphNode::SceneGraphNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) NOEXCEPTRELEASE 
	: id(id), meshPtrs(std::move(meshPtrs)), name(name)
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&this->worldTransform, DirectX::XMMatrixIdentity());
}

void SceneGraphNode::Draw(DxGraphic& graphic, DirectX::FXMMATRIX parentWorldTransform) const NOEXCEPTRELEASE
{
	// 부모를 걸쳐 변한 transform 값을 이 오브젝트를 곱하여 이 오브젝트의 World Transform 값을 구함
	const auto thisWorldTransform = 
		DirectX::XMLoadFloat4x4(&worldTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		parentWorldTransform;

	// Mesh를 그림
	for (const auto meshPtr : meshPtrs)
		meshPtr->Draw(graphic, thisWorldTransform);

	// 자식도 이 노드처럼 그리도록 지시
	for (const auto& child : childPtrs)
		child->Draw(graphic, thisWorldTransform);
}

void SceneGraphNode::ApplyWorldTranfsorm(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&worldTransform, transform);
}

const DirectX::XMFLOAT4X4& SceneGraphNode::GetTranform() const noexcept
{
	return worldTransform;
}

void SceneGraphNode::ShowTree(SceneGraphNode*& selectNode) const noexcept
{
	const int selectId = (selectNode == nullptr) ? -1 : selectNode->GetID();

	const auto currentNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ((GetID() == selectId) ? ImGuiTreeNodeFlags_Selected : 0)
		                                                        | ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

	bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)GetID(), currentNodeFlag, name.c_str());

	// 현재 아이템이 선택되면 선택된 노드로 변경함
	if (ImGui::IsItemClicked())
		selectNode = const_cast<SceneGraphNode*>(this);

	if (isOpen)
	{
		for (const auto& child : childPtrs)
			child->ShowTree(selectNode);

		ImGui::TreePop();
	}
}

int SceneGraphNode::GetID() const noexcept
{
	return id;
}

void SceneGraphNode::AddChild(std::unique_ptr<SceneGraphNode> child) NOEXCEPTRELEASE
{
	assert(child && "추가할 자식 노드가 유효하지 않습니다.");

	childPtrs.push_back(std::move(child));
}

Model::Model(DxGraphic& graphic, const std::string& pathString, const float scale) : modelHierarchy(std::make_unique<ModelHierarchy>())
{
	Assimp::Importer importer;

	const auto model = importer.ReadFile(pathString.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (model == nullptr)
		throw ModelException(__LINE__, __FILE__, importer.GetErrorString());

	for (size_t i = 0; i < model->mNumMeshes; i++)
		meshPtrs.push_back(ConvertMesh(graphic, *model->mMeshes[i], model->mMaterials, pathString, scale));

	int nextID = 0;
	root = ConvertSceneGraphNode(nextID, *model->mRootNode);
}

void Model::Draw(DxGraphic& graphic) const NOEXCEPTRELEASE
{
	if (auto node = modelHierarchy->GetSelectNode())
		node->ApplyWorldTranfsorm(modelHierarchy->GetTransform());

	root->Draw(graphic, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(DxGraphic& graphic, const char* windowName) noexcept
{
	modelHierarchy->ShowWindow(graphic, windowName, *root);
}

void Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
{
	root->ApplyWorldTranfsorm(transform);
}

Model::~Model() noexcept
{
}

std::unique_ptr<Mesh> Model::ConvertMesh(DxGraphic& graphic, const aiMesh& mesh, const aiMaterial* const* materials, const std::filesystem::path& path, float scale)
{
	using VertexCore::VertexLayout;
	using namespace Graphic;

	using namespace std::string_literals;
	const auto texturePath = path.parent_path().string() + "/";
	auto meshTag = texturePath + "%" + mesh.mName.C_Str();

	// 해당 메시에 대해서 랜더링할 수 있도록 바인딩
	std::vector<std::shared_ptr<Render>> bindablePtrs;

	bool hasSpecular = false;
	bool hasNormalMap = false;
	bool hasDiffuse = false;
	bool hasGlassAlpha = false;
	bool hasAlphaDiffuse = false;
	float shininess = 2.0f;

	DirectX::XMFLOAT4 specularColor = { 0.18f, 0.18f, 0.18f, 1.0f };
	DirectX::XMFLOAT4 diffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };

	// =============================================================================
	//						Material에 들어간 이미지 가져오는 기능
	// =============================================================================

	if (mesh.mMaterialIndex >= 0)
	{
		// 해당 메쉬가 사용하는 Material의 해당 번호로 가져옴
		auto& material = *materials[mesh.mMaterialIndex];

		aiString textureFileName;

		// 해당 메쉬에 들어가는 Material의 DIFFUSE에 사용된 이미지 이름을 가져옴
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto texture = Texture::GetRender(graphic, texturePath + textureFileName.C_Str());	// 해당 이미지를 가져옴
			hasAlphaDiffuse = texture->HasAlpha();												// 해당 이미지가 투명도를 가지고 있는지 확인
			bindablePtrs.push_back(std::move(texture));											// 해당 이미지를 렌더링할 수 있도록 바인딩함

			hasDiffuse = true;
		}

		else
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));

		// 해당 메쉬에 들어가는 Material의 SPECULAR에 사용된 이미지 이름을 가져옴
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto texture = Texture::GetRender(graphic, texturePath + textureFileName.C_Str(), 1);
			hasGlassAlpha = texture->HasAlpha();

			bindablePtrs.push_back(std::move(texture));
			hasSpecular = true;
		}

		else
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));

		if (!hasGlassAlpha)
			material.Get(AI_MATKEY_SHININESS, shininess);	// SPECULAR 맵이 없다면 기본 shininess를 사용함

		// 해당 메쉬에 들어가는 Material의 NormalMap에 사용된 이미지 이름을 가져옴
		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto texture = Texture::GetRender(graphic, texturePath + textureFileName.C_Str(), 2);
			hasGlassAlpha = texture->HasAlpha();

			bindablePtrs.push_back(std::move(texture));
			hasNormalMap = true;
		}

		if (hasDiffuse || hasSpecular || hasNormalMap)
			bindablePtrs.push_back(Graphic::SamplerState::GetRender(graphic));
	}

	if (hasDiffuse && hasNormalMap && hasSpecular)
	{
		// Vertex Buffer를 Position3D와 Normal을 갖도록 Vertex Laout를 수정
		VertexCore::VertexBuffer vertexBuffer(std::move(VertexLayout{}.AddType(VertexLayout::Position3D).AddType(VertexLayout::Normal).AddType(VertexLayout::Tangent).AddType(VertexLayout::BiTangent).AddType(VertexLayout::Texture2D)));

		// Vertex Layout에 설정한 정점 정보에 따라서 모델 메시에서 정점과 Normal을 가져옴
		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.emplace_back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		}

		// 메쉬에서 정점 출력 순서를 저장하는 인덱스 가져옴
		std::vector<USHORT> indices;
		indices.reserve(mesh.mNumFaces * 3);

		// 모델 메시에서 모든 면의 정점 순서를 저장함
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3 && "모델의 면이 점 3개로 이루어지지 않음");

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::GetRender(graphic, meshTag, vertexBuffer));
		bindablePtrs.push_back(IndexBuffer::GetRender(graphic, meshTag, indices));

		auto vertexShader = VertexShader::GetRender(graphic, "Shader/TextureNormalMap.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		bindablePtrs.push_back(std::move(vertexShader));

		bindablePtrs.push_back(PixelShader::GetRender(graphic, hasAlphaDiffuse ? "Shader/TextureNormalAlphaLitShader.hlsl" : "Shader/TextureNormalLitShader.hlsl"));
		bindablePtrs.push_back(InputLayout::GetRender(graphic, vertexBuffer.GetVertexLayout(), VSShaderCode));

		SceneGraphNode::PSMaterialConstant psConstant;
		psConstant.specularPower = shininess;
		psConstant.hasGlassMap = hasGlassAlpha ? TRUE : FALSE;

		bindablePtrs.push_back(PixelConstantBuffer<SceneGraphNode::PSMaterialConstant>::GetRender(graphic, psConstant, 1u));
	}

	else if (hasDiffuse && hasNormalMap)
	{
		// Vertex Buffer를 Position3D와 Normal을 갖도록 Vertex Laout를 수정
		VertexCore::VertexBuffer vertexBuffer(std::move(VertexLayout{}.AddType(VertexLayout::Position3D).AddType(VertexLayout::Normal).AddType(VertexLayout::Tangent).AddType(VertexLayout::BiTangent).AddType(VertexLayout::Texture2D)));

		// Vertex Layout에 설정한 정점 정보에 따라서 모델 메시에서 정점과 Normal을 가져옴
		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.emplace_back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		}

		// 메쉬에서 정점 출력 순서를 저장하는 인덱스 가져옴
		std::vector<USHORT> indices;
		indices.reserve(mesh.mNumFaces * 3);

		// 모델 메시에서 모든 면의 정점 순서를 저장함
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3 && "모델의 면이 점 3개로 이루어지지 않음");

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::GetRender(graphic, meshTag, vertexBuffer));
		bindablePtrs.push_back(IndexBuffer::GetRender(graphic, meshTag, indices));

		auto vertexShader = VertexShader::GetRender(graphic, "Shader/TextureNormalMap.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		bindablePtrs.push_back(std::move(vertexShader));

		bindablePtrs.push_back(PixelShader::GetRender(graphic, "Shader/TextureNormalMap.hlsl"));
		bindablePtrs.push_back(InputLayout::GetRender(graphic, vertexBuffer.GetVertexLayout(), VSShaderCode));

		struct PSMaterialConstant
		{
			float specularIntensity;
			float specularPower;
			BOOL normalMapEnable = TRUE;
			float padding[1];
		} matConst;
		matConst.specularPower = shininess;
		matConst.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::GetRender(graphic, matConst, 1u));
	}

	else if (hasDiffuse && !hasNormalMap && hasSpecular)
	{
		// Vertex Buffer를 Position3D와 Normal을 갖도록 Vertex Laout를 수정
		VertexCore::VertexBuffer vertexBuffer(std::move(VertexLayout{}.AddType(VertexLayout::Position3D).AddType(VertexLayout::Normal).AddType(VertexLayout::Texture2D)));

		// Vertex Layout에 설정한 정점 정보에 따라서 모델 메시에서 정점과 Normal을 가져옴
		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.emplace_back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		}

		// 메쉬에서 정점 출력 순서를 저장하는 인덱스 가져옴
		std::vector<USHORT> indices;
		indices.reserve(mesh.mNumFaces * 3);

		// 모델 메시에서 모든 면의 정점 순서를 저장함
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3 && "모델의 면이 점 3개로 이루어지지 않음");

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::GetRender(graphic, meshTag, vertexBuffer));
		bindablePtrs.push_back(IndexBuffer::GetRender(graphic, meshTag, indices));

		auto vertexShader = VertexShader::GetRender(graphic, "Shader/TextureLitSpecular.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		bindablePtrs.push_back(std::move(vertexShader));

		bindablePtrs.push_back(PixelShader::GetRender(graphic, "Shader/TextureLitSpecular.hlsl"));
		bindablePtrs.push_back(InputLayout::GetRender(graphic, vertexBuffer.GetVertexLayout(), VSShaderCode));

		struct PSMaterialConstant
		{
			float specularPower;
			BOOL hasGlass = TRUE;
			float specularMapWeight;
			float padding;
		} matConst;

		matConst.specularPower = shininess;
		matConst.hasGlass = hasGlassAlpha ? TRUE : FALSE;
		matConst.specularMapWeight = 1.0f;

		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::GetRender(graphic, matConst, 1u));
	}

	else if (hasDiffuse)
	{
		// Vertex Buffer를 Position3D와 Normal을 갖도록 Vertex Laout를 수정
		VertexCore::VertexBuffer vertexBuffer(std::move(VertexLayout{}.AddType(VertexLayout::Position3D).AddType(VertexLayout::Normal).AddType(VertexLayout::Texture2D)));

		// Vertex Layout에 설정한 정점 정보에 따라서 모델 메시에서 정점과 Normal을 가져옴
		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			// vertex는 scale에 따라 크게 만들어지도록 설정
			vertexBuffer.emplace_back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		}

		// 메쉬에서 정점 출력 순서를 저장하는 인덱스 가져옴
		std::vector<USHORT> indices;
		indices.reserve(mesh.mNumFaces * 3);

		// 모델 메시에서 모든 면의 정점 순서를 저장함
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3 && "모델의 면이 점 3개로 이루어지지 않음");

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::GetRender(graphic, meshTag, vertexBuffer));
		bindablePtrs.push_back(IndexBuffer::GetRender(graphic, meshTag, indices));

		auto vertexShader = VertexShader::GetRender(graphic, "Shader/TextureLitShader.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		bindablePtrs.push_back(std::move(vertexShader));

		bindablePtrs.push_back(PixelShader::GetRender(graphic, "Shader/TextureLitShader.hlsl"));
		bindablePtrs.push_back(InputLayout::GetRender(graphic, vertexBuffer.GetVertexLayout(), VSShaderCode));

		struct PSMaterialConstant
		{
			float specularIntensity;
			float specularPower;
			float padding[2];
		} matConst;
		matConst.specularPower = shininess;
		matConst.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::GetRender(graphic, matConst, 1u));
	}

	else if (!hasDiffuse && !hasNormalMap && !hasSpecular)
	{
		VertexCore::VertexBuffer vertexBuffer(std::move(VertexLayout{}.AddType(VertexLayout::Position3D).AddType(VertexLayout::Normal)));

		for (UINT i = 0; i < mesh.mNumVertices; i++)
		{
			// vertex는 scale에 따라 크게 만들어지도록 설정
			vertexBuffer.emplace_back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]));
		}

		std::vector<USHORT> indices;
		indices.reserve(mesh.mNumFaces * 3);

		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3 && "모델의 면이 점 3개로 이루어지지 않음");

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::GetRender(graphic, meshTag, vertexBuffer));
		bindablePtrs.push_back(IndexBuffer::GetRender(graphic, meshTag, indices));

		auto vertexShader = VertexShader::GetRender(graphic, "Shader/LitShader.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		bindablePtrs.push_back(std::move(vertexShader));

		bindablePtrs.push_back(PixelShader::GetRender(graphic, "Shader/LitShader.hlsl"));
		bindablePtrs.push_back(InputLayout::GetRender(graphic, vertexBuffer.GetVertexLayout(), VSShaderCode));

		SceneGraphNode::PSMaterialConstantNoTexture matConst;
		matConst.specularPower = shininess;
		matConst.specularColor = specularColor;
		matConst.materialColor = diffuseColor;

		bindablePtrs.push_back(PixelConstantBuffer<SceneGraphNode::PSMaterialConstantNoTexture>::GetRender(graphic, matConst, 1u));
	}

	else
		throw std::runtime_error("해당 이미지 조합으로 적절한 Material을 만들 수 없음");

	bindablePtrs.push_back(Rasterizer::GetRender(graphic, hasAlphaDiffuse));
	bindablePtrs.push_back(ColorBlend::GetRender(graphic, false));

	return std::make_unique<Mesh>(graphic, std::move(bindablePtrs));
}

std::unique_ptr<SceneGraphNode> Model::ConvertSceneGraphNode(int& nextID, const aiNode& modelNode) NOEXCEPTRELEASE
{
	const auto transform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&modelNode.mTransformation)));

	std::vector<Mesh*> currentMeshPtrs;
	currentMeshPtrs.reserve(modelNode.mNumMeshes);

	for (size_t i = 0; i < modelNode.mNumMeshes; i++)
	{
		const auto meshIndex = modelNode.mMeshes[i];
		currentMeshPtrs.push_back(meshPtrs.at(meshIndex).get());
	}

	auto node = std::make_unique<SceneGraphNode>(nextID++, modelNode.mName.C_Str(), std::move(currentMeshPtrs), transform);

	for (size_t i = 0; i < modelNode.mNumChildren; i++)
		node->AddChild(ConvertSceneGraphNode(nextID, *modelNode.mChildren[i]));

	return node;
}

ModelException::ModelException(int line, const char* file, std::string note) noexcept : BaseException(line, file), note(std::move(note))
{

}

const char* ModelException::what() const noexcept
{
	std::ostringstream out;
	out << BaseException::what() << std::endl << "[Exception] " << GetNote();

	exceptionText = out.str();

	return exceptionText.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}