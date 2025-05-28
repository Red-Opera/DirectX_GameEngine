#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorCubeObject : public ColorObject
{
public:
	ColorCubeObject(std::shared_ptr<class Object> object);

	virtual std::string GetClassName() const { return "ColorCubeObject"; }
	static std::string GetStaticClassName() { return "ColorCubeObject"; }

	void Initialize() override;
};