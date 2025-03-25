#pragma once

#include "../../DxGraphic.h"

#include "Core/Camera/CameraViewFrustumCulling.h"
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
	Drawable(const Material& material, const aiMesh& mesh, float scale = 1.0f) noexcept;
	Drawable(const Drawable&) = delete;

	void Submit(size_t channelFilter) const noexcept;
	void Accept(TechniqueBase& tech);
	void SetRenderPipeline() const NOEXCEPTRELEASE;
	UINT GetIndexCount() const NOEXCEPTRELEASE;

	void AddTechnique(Technique technique) noexcept;
	void LinkTechniques(RenderGraphNameSpace::RenderGraph&);

	void CalculateBoundingSphere(const aiMesh& mesh, float scale) noexcept;
	DirectX::XMFLOAT3 GetBoundingSphereCenter() const noexcept;
	float GetBoundingSphereRadius() const noexcept;

	// View Frustum Culling 체크
	bool IsInViewFrustum(const CameraViewFrustumCulling& viewFrustum) const noexcept;

	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;	// 오브젝트의 Transform의 행렬을 반환
	virtual ~Drawable();

protected:
	std::vector<Technique> techniques;

	std::shared_ptr<Graphic::IndexBuffer> indexBuffer;
	std::shared_ptr<Graphic::VertexBuffer> vertexBuffer;
	std::shared_ptr<Graphic::PrimitiveTopology> primitiveTopology;

	// 바운딩 구 정보 추가
	DirectX::XMFLOAT3 m_boundingSphereCenter = { 0.0f, 0.0f, 0.0f };
	float m_boundingSphereRadius = 1.0f;
};

