#include "stdafx.h"
#include "ColorConeObject.h"

#include "../BaseModel/ConeFrame.h"

ColorConeObject::ColorConeObject(float scale, GraphicResource::Image::Color color, bool isLit)
	: ColorConeObject(Scale{ scale, scale, scale }, color, isLit)
{

}

ColorConeObject::ColorConeObject(Scale scale, GraphicResource::Image::Color color, bool isLit)
{
	auto model = ConeFrame::CreateTextureFrameSeparateBottom();

	SetRenderingPipeline(scale, color, isLit, model);
}