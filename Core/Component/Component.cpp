#include "stdafx.h"
#include "Component.h"

#include "Core/Object/Object.h"
#include "Core/Component/Transform/TransformComponent.h"

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

void Component::SetEnable(bool isEnable)
{
    if (this->isActive == isEnable)
        return;
        
    bool wasActive = this->isActive;
    this->isActive = isEnable;
    
    // 상태가 실제로 변경되었을 때만 이벤트 호출
    if (wasActive != isEnable)
    {
        if (isEnable)
            OnEnable();

        else
            OnDisable();
    }
}