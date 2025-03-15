#pragma once

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"
#include "Core/Draw/Base/Image/Image.h"

#include "Utility/Vector.h"

class ColorSphereObject : public Drawable
{
public:
	ColorSphereObject(float size, GraphicResource::Image::Color color, bool isLit = true);

	void SetPosition(Vector3 position) noexcept;
	void SetRotation(Rotation rotation) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;

	void CreateControlWindow(const char* name) noexcept;

private:
	Vector3 position = { 1.0f, 1.0f, 1.0f };
	Rotation rotation = { 0.0f, 0.0f, 0.0f };

	int size;
};