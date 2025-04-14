#pragma once

#include "Core/Scene/Base/Scene.h"

class EmptyScene : public Scene
{
public:
	EmptyScene(std::string sceneName);

	static std::shared_ptr<Scene> Create(std::string sceneName);

	void Initialize() override;
};