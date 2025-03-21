#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorCylinderObject : public ColorObject
{
public:
	ColorCylinderObject(float scale = 1.0f, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
	ColorCylinderObject(Scale scale, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
};