#pragma once

#include "../../DxGraphic.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/Technique.h"

#include <memory>
#include <DirectXMath.h>

class TechniqueBase;
class Material;
struct aiMesh;

namespace RenderGraphNameSpace { class RenderGraph; }

namespace Graphic
{
	class IndexBuffer;
	class VertexBuffer;
	class PrimitiveTopology;
	class InputLayout;
}

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(DxGraphic& graphic, const Material& material, const aiMesh& mesh, float scale = 1.0f) noexcept;
	Drawable(const Drawable&) = delete;

	void Submit() const noexcept;
	void Accept(TechniqueBase& tech);
	void SetRenderPipeline(DxGraphic& graphic) const NOEXCEPTRELEASE;
	UINT GetIndexCount() const NOEXCEPTRELEASE;

	void AddTechnique(Technique technique) noexcept;
	void LinkTechniques(RenderGraphNameSpace::RenderGraph&);

	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;	// 오브젝트의 Transform의 행렬을 반환
	virtual ~Drawable();

protected:
	std::vector<Technique> techniques;

	std::shared_ptr<Graphic::IndexBuffer> indexBuffer;
	std::shared_ptr<Graphic::VertexBuffer> vertexBuffer;
	std::shared_ptr<Graphic::PrimitiveTopology> primitiveTopology;
};

