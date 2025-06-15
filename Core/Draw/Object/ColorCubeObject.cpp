#include "stdafx.h"
#include "ColorCubeObject.h"

#include "Core/Object/Object.h"
#include "../BaseModel/CubeFrame.h"

#include "Core/Draw/Base/TriangleIndexList.h"

ColorCubeObject::ColorCubeObject(std::shared_ptr<class Object> object)
    : ColorObject(object)
{
    // BaseModel 타입 설정
    object->SetBaseModelType(BaseModelType::Cube);
}

void ColorCubeObject::Initialize()
{
    auto model = isLit ? CubeFrame::CreateTextureFrame() : CubeFrame::CreateFrame();

    SetRenderingPipeline(color, isLit, model);

    ColorObject::Initialize();
}