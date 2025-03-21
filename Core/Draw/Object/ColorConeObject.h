#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorConeObject : public ColorObject
{
public:
	ColorConeObject(float scale = 1.0f, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
	ColorConeObject(Scale scale, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
};