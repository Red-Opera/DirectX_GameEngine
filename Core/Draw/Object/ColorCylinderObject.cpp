#include "stdafx.h"
#include "ColorCylinderObject.h"

#include "../BaseModel/CylinderFrame.h"

ColorCylinderObject::ColorCylinderObject(std::shared_ptr<class Object> object)
	: ColorObject(object)
{

}

void ColorCylinderObject::Initialize()
{
	auto model = isLit ? CylinderFrame::CreateTextureFrame() : CylinderFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}