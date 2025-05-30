#include "stdafx.h"
#include "ColorCubeObject.h"

#include "../BaseModel/CubeFrame.h"

#include "Core/Draw/Base/TriangleIndexList.h"

ColorCubeObject::ColorCubeObject(std::shared_ptr<class Object> object)
	: ColorObject(object)
{

}

void ColorCubeObject::Initialize()
{
	auto model = isLit ? CubeFrame::CreateTextureFrame() : CubeFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}