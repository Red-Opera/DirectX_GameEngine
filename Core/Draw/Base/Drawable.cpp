#include "stdafx.h"
#include "Drawable.h"

#include "../../RenderingPipeline/Bindable.h"
#include "../../RenderingPipeline/IA/IndexBuffer.h"

void Drawable::Draw(DxGraphic& graphic) const noexcept(!_DEBUG)
{
	for (auto& b : binds)
		b->PipeLineSet(graphic);
	
	for (auto& b : GetStaticBinds())
		b->PipeLineSet(graphic);

	graphic.DrawIndexed(indexBuffer->GetIndexCount());
}

void Drawable::AddBind(unique_ptr<Bindable> bind) noexcept(!_DEBUG)
{
	assert("AddIndexBuffer�� ���ؼ� Index Buffer�� ������ �մϴ�." && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(move(bind));
}

void Drawable::AddIndexBuffer(unique_ptr<IndexBuffer> indexBuffer) noexcept(!_DEBUG)
{
	assert("�ε��� ���۰� �������� ����. �ε��� ���۸� ������ �մϴ�." && this->indexBuffer == nullptr);

	this->indexBuffer = indexBuffer.get();
	binds.push_back(move(indexBuffer));
}