#include "stdafx.h"
#include "ColorPlaneObject.h"

#include "../BaseModel/ColorPlaneFrame.h"

#include "Core/Object/Object.h"
#include "Core/Component/Transform/TransformComponent.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

ColorPlaneObject::ColorPlaneObject(std::shared_ptr<Object> object)
	: ColorObject(object)
{

}

void ColorPlaneObject::Initialize()
{
	TriangleIndexList model = isLit ? ColorPlaneFrame::CreateTextureFrame() : ColorPlaneFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}