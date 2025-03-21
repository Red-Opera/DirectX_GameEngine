#include "stdafx.h"
#include "ColorCubeObject.h"

#include "../BaseModel/CubeFrame.h"

#include "Core/Draw/Base/TriangleIndexList.h"

ColorCubeObject::ColorCubeObject(float scale, GraphicResource::Image::Color color, bool isLit)
	: ColorCubeObject(Scale{ scale, scale, scale }, color, isLit)
{

}

ColorCubeObject::ColorCubeObject(Scale scale, GraphicResource::Image::Color color, bool isLit)
{
	auto model = isLit ? CubeFrame::CreateTextureFrame() : CubeFrame::CreateFrame();

	if (isLit)
		model.SetNormalVector();

	SetRenderingPipeline(scale, color, isLit, model);
}