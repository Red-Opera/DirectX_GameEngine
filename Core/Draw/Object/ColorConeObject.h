#pragma once

#include "Core/Draw/Object/ColorObject.h"

#include <memory>

class ColorConeObject : public ColorObject
{
public:
	ColorConeObject(std::shared_ptr<class Object> object);

	virtual std::string GetClassName() const { return "ColorConeObject"; }
	static std::string GetStaticClassName() { return "ColorConeObject"; }

	void Initialize() override;
};