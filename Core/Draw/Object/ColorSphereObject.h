#pragma once

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"
#include "Core/Draw/Base/Image/Image.h"

#include "Utility/Vector.h"

class ColorSphereObject : public Drawable
{
public:
	ColorSphereObject(Scale scale, GraphicResource::Image::Color color, bool isLit = true);

	void SetPosition(Vector3 position) noexcept;
	void SetRotation(Rotation rotation) noexcept;
	void SetScale(Scale scale) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;

	void CreateControlWindow(const char* name) noexcept;

private:
	Vector3 position = { 1.0f, 1.0f, 1.0f };
	Rotation rotation = { 0.0f, 0.0f, 0.0f };
	Scale scale = { 1.0f, 1.0f, 1.0f };
};