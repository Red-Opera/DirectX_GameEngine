#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorCylinderObject : public ColorObject
{
public:
	ColorCylinderObject(std::shared_ptr<class Object> object);

	virtual std::string GetClassName() const { return "ColorCylinderObject"; }
	static std::string GetStaticClassName() { return "ColorCylinderObject"; }

	void Initialize() override;
};