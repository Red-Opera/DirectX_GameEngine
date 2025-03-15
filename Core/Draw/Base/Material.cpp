#include "stdafx.h"
#include "Material.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TransformConstantBufferScaling.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include <filesystem>

Material::Material(const aiMaterial& material, const std::filesystem::path& path) NOEXCEPTRELEASE
	: modelPath(path.string())
{
	using namespace Graphic;

	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		name = tempName.C_Str();
	}

	{
		Technique light{ "Light", RenderingChannel::main };
		RenderStep renderStep("lambertian");

		std::string shaderCodeName = "Shader/LitTexture";
		aiString textureName;

		vertexLayout.AddType(VertexCore::VertexLayout::Position3D);
		vertexLayout.AddType(VertexCore::VertexLayout::Normal);

		DynamicConstantBuffer::EditLayout layout;
		
		bool hasTexture = false;
		bool hasGlassAlpha = false;

		// Diffuse
		{
			bool hasAlpha = false;

			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureName) == aiReturn_SUCCESS)
			{
				hasTexture = true;

				shaderCodeName += "Diffuse";
				vertexLayout.AddType(VertexCore::VertexLayout::Texture2D);

				auto texture = Texture::GetRender(rootPath + textureName.C_Str());

				if (texture->HasAlpha())
				{
					hasAlpha = true;
					shaderCodeName += "Mask";
				}

				renderStep.AddRender(std::move(texture));
			}

			else
				layout.add<DynamicConstantBuffer::float3>("materialColor");

			renderStep.AddRender(Rasterizer::GetRender(hasAlpha));
		}

		// Specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureName) == aiReturn_SUCCESS)
			{
				hasTexture = true;

				shaderCodeName += "Specular";
				vertexLayout.AddType(VertexCore::VertexLayout::Texture2D);

				auto texture = Texture::GetRender(rootPath + textureName.C_Str(), 1);

				hasGlassAlpha = texture->HasAlpha();
				renderStep.AddRender(std::move(texture));

				layout.add<DynamicConstantBuffer::Bool>("useGlassAlpha");
				layout.add<DynamicConstantBuffer::Bool>("useSpecularMap");
			}

			layout.add<DynamicConstantBuffer::float3>("specularColor");
			layout.add<DynamicConstantBuffer::float1>("specularPower");
			layout.add<DynamicConstantBuffer::float1>("specularGlass");
		}

		// Normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &textureName) == aiReturn_SUCCESS)
			{
				hasTexture = true;

				shaderCodeName += "Normal";
				vertexLayout.AddType(VertexCore::VertexLayout::Texture2D);
				vertexLayout.AddType(VertexCore::VertexLayout::Tangent);
				vertexLayout.AddType(VertexCore::VertexLayout::BiTangent);

				renderStep.AddRender(Texture::GetRender(rootPath + textureName.C_Str(), 2));

				layout.add<DynamicConstantBuffer::Bool>("useNormalMap");
				layout.add<DynamicConstantBuffer::float1>("normalMapPower");
			}
		}

		{
			// Shader 코드 이름이 LitTexture인 경우 LitColor로 변경
			if (shaderCodeName == "Shader/LitTexture")
				shaderCodeName = "Shader/LitColor";

			renderStep.AddRender(std::make_shared<TransformConstantBuffer>(0u));

			auto vertexShader = VertexShader::GetRender(shaderCodeName + ".hlsl");
			renderStep.AddRender(InputLayout::GetRender(vertexLayout, *vertexShader));
			renderStep.AddRender(std::move(vertexShader));
			renderStep.AddRender(PixelShader::GetRender(shaderCodeName + ".hlsl"));

			if (hasTexture)
				renderStep.AddRender(Graphic::SamplerState::GetRender());

			DynamicConstantBuffer::Buffer buffer{ std::move(layout) };

			if (auto materialColor = buffer["materialColor"]; materialColor.IsExist())
			{
				aiColor3D color = { 0.45f, 0.45f, 0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);

				materialColor = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}

			buffer["useGlassAlpha"].SetValue(hasGlassAlpha);
			buffer["useSpecularMap"].SetValue(true);

			if (auto specularColor = buffer["specularColor"]; specularColor.IsExist())
			{
				aiColor3D color = { 0.18f, 0.18f, 0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);

				specularColor = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}

			buffer["specularPower"].SetValue(1.0f);

			if (auto specularGlass = buffer["specularGlass"]; specularGlass.IsExist())
			{
				float glass = 8.0f;
				material.Get(AI_MATKEY_SHININESS, glass);
				specularGlass = glass;
			}

			buffer["useNormalMap"].SetValue(true);
			buffer["normalMapPower"].SetValue(1.0f);
			renderStep.AddRender(std::make_unique<Graphic::CachingPixelConstantBufferEx>(std::move(buffer), 1u));
		}

		light.push_back(std::move(renderStep));
		techniques.push_back(std::move(light));
	}

	// Outline
	{
		Technique outline{ "Outline", RenderingChannel::main, false };
		{
			RenderStep mask("outlineMask");

			mask.AddRender(
				InputLayout::GetRender(
					vertexLayout, 
					*VertexShader::GetRender("Shader/ColorShader.hlsl")
				)
			);

			mask.AddRender(std::make_shared<TransformConstantBuffer>());
	
			outline.push_back(std::move(mask));
		}
	
		{
			RenderStep draw("outlineDraw");

			{
				DynamicConstantBuffer::EditLayout layout;
				layout.add<DynamicConstantBuffer::float3>("materialColor");
	
				auto buffer = DynamicConstantBuffer::Buffer(std::move(layout));
				buffer["materialColor"] = DirectX::XMFLOAT3{ 1.0f, 0.4f, 0.4f };
	
				draw.AddRender(std::make_shared<Graphic::CachingPixelConstantBufferEx>(buffer, 1u));
			}
	
			draw.AddRender(
				InputLayout::GetRender(
					vertexLayout,
					*VertexShader::GetRender("Shader/ColorShader.hlsl")
				)
			);

			draw.AddRender(std::make_shared<TransformConstantBuffer>());
	
			outline.push_back(std::move(draw));
		}
	
		techniques.push_back(std::move(outline));
	}

	// 그림자 맵 Technique
	{
		Technique shadowMap{ "ShadowMap", RenderingChannel::shadow, true };
		{
			RenderStep draw("ShadowMap");
	
			draw.AddRender(InputLayout::GetRender(vertexLayout, *VertexShader::GetRender("Shader/ColorShader.hlsl")));
			draw.AddRender(std::make_shared<TransformConstantBuffer>());
	
			shadowMap.push_back(std::move(draw));
		}
	
		techniques.push_back(std::move(shadowMap));
	}
}

VertexCore::VertexBuffer Material::GetVertex(const aiMesh& mesh) const noexcept
{
	return { vertexLayout, mesh };
}

std::vector<unsigned short> Material::GetIndex(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);

	for (UINT i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	return indices;
}

std::shared_ptr<Graphic::VertexBuffer> Material::CreateVertexBuffer(const aiMesh& mesh, float scale) const NOEXCEPTRELEASE
{
	auto vertexBuffer = GetVertex(mesh);

	if (scale != 1.0f)
	{
		for (auto i = 0u; i < vertexBuffer.count(); i++)
		{
			DirectX::XMFLOAT3& position = vertexBuffer[i].GetValue<VertexCore::VertexLayout::VertexType::Position3D>();

			position.x *= scale;
			position.y *= scale;
			position.z *= scale;
		}
	}

	return Graphic::VertexBuffer::GetRender(CreateMeshTag(mesh), std::move(vertexBuffer));
}

std::shared_ptr<Graphic::IndexBuffer> Material::CreateIndexBuffer(const aiMesh& mesh) const NOEXCEPTRELEASE
{
	return Graphic::IndexBuffer::GetRender(CreateMeshTag(mesh), GetIndex(mesh));
}

std::vector<Technique> Material::GetTechnique() const noexcept
{
	return techniques;
}

std::string Material::CreateMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}