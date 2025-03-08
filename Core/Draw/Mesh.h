#pragma once


#include "Core/DxGraphic.h"
#include "Core/Draw/Base/Drawable.h"
#include "Core/Exception/WindowException.h"

class FrameCommander;
class Material;
struct aiMesh;

// 오브젝트 일부분의 Mesh 부분을 역할하는 클래스
class Mesh : public Drawable
{
public:
	Mesh(const Material& material, const aiMesh& mesh, float scale = 1.0f) NOEXCEPTRELEASE;

	void Submit(size_t channel, DirectX::XMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;	// Mesh의 World 행렬을 반환함

private:
	mutable DirectX::XMFLOAT4X4 transform;	// Mesh의 World 행렬을 저장하는 행렬
};