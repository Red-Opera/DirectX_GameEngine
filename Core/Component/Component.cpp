#include "stdafx.h"
#include "Component.h"

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"

Component::Component(std::shared_ptr<Object> object)
{
	SetObject(object);

	if (object && object->transform != nullptr)
		transform = object->transform;
}

void Component::SetObject(std::shared_ptr<Object> object)
{
	this->object = object;
}

std::shared_ptr<class Object> Component::GetObject()
{
	return object;
}