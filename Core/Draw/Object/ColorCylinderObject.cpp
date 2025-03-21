#include "stdafx.h"
#include "ColorCylinderObject.h"

#include "../BaseModel/CylinderFrame.h"

ColorCylinderObject::ColorCylinderObject(float scale, GraphicResource::Image::Color color, bool isLit)
	: ColorCylinderObject(Scale{ scale, scale, scale }, color, isLit)
{

}

ColorCylinderObject::ColorCylinderObject(Scale scale, GraphicResource::Image::Color color,bool isLit)
{
	auto model = isLit ? CylinderFrame::CreateTextureFrame() : CylinderFrame::CreateFrame();

	SetRenderingPipeline(scale, color, isLit, model);
}