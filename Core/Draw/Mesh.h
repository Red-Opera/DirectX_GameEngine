#pragma once


#include "Core/DxGraphic.h"
#include "Core/Draw/Base/Drawable.h"
#include "Core/Exception/WindowException.h"

class FrameCommander;
class Material;
struct aiMesh;

// ������Ʈ �Ϻκ��� Mesh �κ��� �����ϴ� Ŭ����
class Mesh : public Drawable
{
public:
	Mesh(const Material& material, const aiMesh& mesh, float scale = 1.0f) NOEXCEPTRELEASE;

	void Submit(size_t channel, DirectX::XMMATRIX parentWorldTransform) const NOEXCEPTRELEASE;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;	// Mesh�� World ����� ��ȯ��

private:
	mutable DirectX::XMFLOAT4X4 transform;	// Mesh�� World ����� �����ϴ� ���
};