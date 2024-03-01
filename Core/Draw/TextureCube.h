#pragma once

#include "TestObject.h"

#include <random>

class TextureCube : public TestObject<TextureCube>
{
public:
	TextureCube(DxGraphic& graphic, std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);
};