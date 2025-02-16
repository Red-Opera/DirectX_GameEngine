#include "stdafx.h"
#include "Drawable.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include <assimp/scene.h>

using namespace Graphic;

Drawable::Drawable(DxGraphic& graphic, const Material& material, const aiMesh& mesh, float scale) noexcept
{
	vertexBuffer = material.CreateVertexBuffer(graphic, mesh, scale);
	indexBuffer = material.CreateIndexBuffer(graphic, mesh);
	primitiveTopology = Graphic::PrimitiveTopology::GetRender(graphic);

	for (auto& tech : material.GetTechnique())
		AddTechnique(std::move(tech));
}

void Drawable::Submit() const noexcept
{
	for (const auto& technique : techniques)
		technique.Submit(*this);
}

void Drawable::Accept(TechniqueBase& tech)
{
	for (auto& technique : techniques)
		technique.Accept(tech);
}

void Drawable::SetRenderPipeline(DxGraphic& graphic) const NOEXCEPTRELEASE
{
	primitiveTopology->SetRenderPipeline(graphic);
	indexBuffer->SetRenderPipeline(graphic);
	vertexBuffer->SetRenderPipeline(graphic);
}

UINT Drawable::GetIndexCount() const NOEXCEPTRELEASE
{
	return indexBuffer->GetIndexCount();
}

void Drawable::AddTechnique(Technique technique) noexcept
{
	technique.InitializeParentReferences(*this);
	techniques.push_back(std::move(technique));
}

void Drawable::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	for (auto& tech : techniques)
		tech.Link(renderGraph);
}

Drawable::~Drawable()
{

}