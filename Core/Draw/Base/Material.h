#pragma once

#include "Core/DxGraphic.h"

#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/Technique.h"

#include <filesystem>
#include <vector>

struct aiMaterial;
struct aiMesh;

namespace Graphic
{
	class VertexBuffer;
	class IndexBuffer;
};

class Material
{
public:
	Material(DxGraphic& graphic, const aiMaterial& material, const std::filesystem::path& path) NOEXCEPTRELEASE;

	VertexCore::VertexBuffer GetVertex(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> GetIndex(const aiMesh& mesh) const noexcept;

	std::shared_ptr<Graphic::VertexBuffer> CreateVertexBuffer(DxGraphic& graphic, const aiMesh& mesh, float scale = 1.0f) const NOEXCEPTRELEASE;

	std::shared_ptr<Graphic::IndexBuffer> CreateIndexBuffer(DxGraphic& graphic, const aiMesh& mesh) const NOEXCEPTRELEASE;

	std::vector<Technique> GetTechnique() const noexcept;

private:
	std::string CreateMeshTag(const aiMesh& mesh) const noexcept;

	VertexCore::VertexLayout vertexLayout;
	std::vector<Technique> techniques;

	std::string modelPath;
	std::string name;
};