#include "stdafx.h"
#include "Drawable.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include <External/Assimp/scene.h>

using namespace Graphic;

Drawable::Drawable(const Material& material, const aiMesh& mesh, float scale) noexcept
{
	vertexBuffer = material.CreateVertexBuffer(mesh, scale);
	indexBuffer = material.CreateIndexBuffer(mesh);
	primitiveTopology = Graphic::PrimitiveTopology::GetRender();

	for (auto& tech : material.GetTechnique())
		AddTechnique(std::move(tech));
}

void Drawable::Submit(size_t channelFilter) const noexcept
{
	for (const auto& technique : techniques)
		technique.Submit(*this, channelFilter);
}

void Drawable::Accept(TechniqueBase& tech)
{
	for (auto& technique : techniques)
		technique.Accept(tech);
}

void Drawable::SetRenderPipeline() const NOEXCEPTRELEASE
{
	primitiveTopology->SetRenderPipeline();
	indexBuffer->SetRenderPipeline();
	vertexBuffer->SetRenderPipeline();
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