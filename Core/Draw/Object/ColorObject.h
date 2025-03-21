#pragma once

#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"
#include "Core/Draw/Base/Image/Image.h"

#include "Utility/Vector.h"

class ColorObject : public Drawable
{
public:
	ColorObject() = default;

	void SetRenderingPipeline(Scale scale, GraphicResource::Image::Color color, bool isLit, class TriangleIndexList model);

	void SetPosition(Vector3 position) noexcept;
	void SetRotation(Rotation rotation) noexcept;
	void SetScale(Scale scale) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;

	virtual void CreateControlWindow(const char* name) noexcept;

protected:
	Vector3 position = { 1.0f, 1.0f, 1.0f };
	Rotation rotation = { 0.0f, 0.0f, 0.0f };
	Scale scale = { 1.0f, 1.0f, 1.0f };
};