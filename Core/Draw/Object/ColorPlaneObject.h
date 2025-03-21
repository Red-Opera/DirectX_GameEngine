#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorPlaneObject : public ColorObject
{
public:
	ColorPlaneObject(float scale = 1.0f, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
	ColorPlaneObject(Vector2 scale, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);

	void CreateControlWindow(const char* name) noexcept override;
};