#include "stdafx.h"
#include "Drawable.h"

#include "../../RenderingPipeline/Render.h"
#include "../../RenderingPipeline/IA/IndexBuffer.h"
using namespace std;
using namespace Graphic;

void Drawable::Draw(DxGraphic& graphic) const NOEXCEPTRELEASE
{
	for (auto& b : binds)
		b->PipeLineSet(graphic);

	graphic.DrawIndexed(indexBuffer->GetIndexCount());
}

void Drawable::AddBind(shared_ptr<Render> bind) NOEXCEPTRELEASE
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("타켓티하는 Render에 IndexBuffer가 존재하지 않음" && indexBuffer == nullptr);
		indexBuffer = &static_cast<IndexBuffer&>(*bind);
;	}

	binds.push_back(move(bind));
}