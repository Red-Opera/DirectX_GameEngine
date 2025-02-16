#pragma once

class ModelBase
{
public:
	virtual bool push(class SceneGraphNode& node) = 0;
	virtual void pop(class SceneGraphNode& node) = 0;
};