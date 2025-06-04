#include "stdafx.h"
#include "ColorSphereObject.h"

#include "Core/Object/Object.h"
#include "../BaseModel/SphereFrame.h"

ColorSphereObject::ColorSphereObject(std::shared_ptr<class Object> object)
	: ColorObject(object)
{
	object->SetBaseModelType(BaseModelType::Sphere);
}

void ColorSphereObject::Initialize()
{
	auto model = isLit ? SphereFrame::CreateTextureFrame() : SphereFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}