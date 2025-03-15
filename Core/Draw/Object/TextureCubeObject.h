#pragma once

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"

#include <string>

class TextureCubeObject : public Drawable
{
public:
	TextureCubeObject(float size, std::string path);

	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;

	// Drawable을(를) 통해 상속됨
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

	void SpawnControlWindow(const char* name) noexcept;

private:
	DirectX::XMFLOAT3 position = { 1.0f, 1.0f, 1.0f };

	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};