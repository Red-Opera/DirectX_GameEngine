#include "stdafx.h"
#include "ColorSphereObject.h"

#include "../BaseModel/SphereFrame.h"

ColorSphereObject::ColorSphereObject(std::shared_ptr<class Object> object)
	: ColorObject(object)
{

}

void ColorSphereObject::Initialize()
{
	auto model = isLit ? SphereFrame::CreateTextureFrame() : SphereFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}