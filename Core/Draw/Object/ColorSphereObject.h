#pragma once

#include "ColorObject.h"

class ColorSphereObject : public ColorObject
{
public:
	ColorSphereObject(float scale = 1.0f, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
	ColorSphereObject(Scale scale, GraphicResource::Image::Color color = { 255, 255, 255 }, bool isLit = true);
};