#include "stdafx.h"
#include "ColorSphereObject.h"

#include "../BaseModel/SphereFrame.h"

ColorSphereObject::ColorSphereObject(float scale, GraphicResource::Image::Color color, bool isLit)
	: ColorSphereObject(Scale{ scale, scale, scale }, color, isLit)
{

}

ColorSphereObject::ColorSphereObject(Scale scale, GraphicResource::Image::Color color, bool isLit)
{
	auto model = isLit ? SphereFrame::CreateTextureFrame() : SphereFrame::CreateFrame();

	SetRenderingPipeline(scale, color, isLit, model);
}