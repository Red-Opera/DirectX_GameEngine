#pragma once

#include "Core/Scene/Base/Scene.h"

class SponzaScene : public Scene
{
public:
	SponzaScene(std::string sceneName);

	void Initialize() override;

	static std::shared_ptr<Scene> Create(std::string sceneName);
};