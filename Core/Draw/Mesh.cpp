#include "stdafx.h"
#include "Mesh.h"

#include "Base/Image/Image.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TypeCache.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"

#include "Utility/MathInfo.h"

#include <filesystem>
#include <unordered_map>
#include <sstream>

Mesh::Mesh(DxGraphic& graphic, const Material& material, const aiMesh& mesh, float scale) NOEXCEPTRELEASE 
	: Drawable(graphic, material, mesh, scale)
{

}

void Mesh::Submit(DirectX::XMMATRIX parentWorldTransform) const NOEXCEPTRELEASE
{
	DirectX::XMStoreFloat4x4(&transform, parentWorldTransform);
	Drawable::Submit();
}

DirectX::XMMATRIX Mesh::GetTransformMatrix() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}