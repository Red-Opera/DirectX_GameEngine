#pragma once

#include "ColorObject.h"

class ColorSphereObject : public ColorObject
{
public:
	ColorSphereObject(std::shared_ptr<class Object> object);

	virtual std::string GetClassName() const { return "ColorSphereObject"; }
	static std::string GetStaticClassName() { return "ColorSphereObject"; }

	void Initialize() override;
};