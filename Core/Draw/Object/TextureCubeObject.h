#pragma once

#include "Core/Draw/Object/ColorObject.h"

#include <string>

class TextureCubeObject : public ColorObject
{
public:
	TextureCubeObject(std::shared_ptr<class Object> object, std::string path);

	void SetTexturePath(std::string path) { this->path = path; }
	std::string GetTexturePath() const { return path; }

	virtual std::string GetClassName() const { return "TextureCubeObject"; }
	static std::string GetStaticClassName() { return "TextureCubeObject"; }

	void Initialize() override;
	
private:
	std::string path;
};