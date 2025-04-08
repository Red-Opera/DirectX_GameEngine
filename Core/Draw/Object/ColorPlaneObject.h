#pragma once

#include "Core/Draw/Object/ColorObject.h"

class ColorPlaneObject : public ColorObject
{
public:
	ColorPlaneObject(std::shared_ptr<class Object> object);

	virtual std::string GetClassName() const { return "ColorPlaneObject"; }
	static std::string GetStaticClassName() { return "ColorPlaneObject"; }

	void Initialize() override;
};