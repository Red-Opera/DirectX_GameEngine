#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorCubeObject : public ColorObject
{
public:
	ColorCubeObject(float scale = 1.0f, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
	ColorCubeObject(Scale scale, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
};