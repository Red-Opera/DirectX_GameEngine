#include "stdafx.h"
#include "ColorConeObject.h"

#include "../BaseModel/ConeFrame.h"

ColorConeObject::ColorConeObject(std::shared_ptr<class Object> object)
	: ColorObject(object)
{

}

void ColorConeObject::Initialize()
{
	auto model = isLit ? ConeFrame::CreateTextureFrameSeparateBottom() : ConeFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}