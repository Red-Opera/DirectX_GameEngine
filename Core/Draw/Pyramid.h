#pragma once
#include "Base/DrawableBase.h"
#include <random>

class Pyramid : public DrawableBase<Pyramid>
{
public:
	Pyramid(DxGraphic& graphic, std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// 위치 및 회전 
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// speed (delta/s)
	float deltaRoll;
	float deltaPitch;
	float deltaYaw;
	float deltaTheta;
	float deltaPhi;
	float deltaChi;
};