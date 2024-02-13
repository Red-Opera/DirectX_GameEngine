#pragma once
#include "Base/DrawableBase.h"

#include <random>

class Sheet : public DrawableBase<Sheet>
{
public:
	Sheet(DxGraphic& graphic, std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	// DrawableBase을(를) 통해 상속됨
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Update(float dt) noexcept override;

private:
	// Transform
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;

	float theta;
	float phi;
	float chi;

	// Speed (Delta/s)
	float deltaRoll;
	float deltaPitch;
	float deltaYaw;

	float deltaTheta;
	float deltaPhi;
	float deltaChi;
};

