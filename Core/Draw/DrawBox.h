#pragma once
#include "Base/DrawableBase.h"

#include <random>

class DrawBox : public DrawableBase<DrawBox>
{
public:
	DrawBox(DxGraphic& graphic, mt19937& random,
		uniform_real_distribution<float>& adist,
		uniform_real_distribution<float>& ddist,
		uniform_real_distribution<float>& odist,
		uniform_real_distribution<float>& rdist,
		uniform_real_distribution<float>& bdist);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	// Position Rotation
	float r;
	float roll = 0.0f;		// X축 기준 회전
	float pitch = 0.0f;		// Y축 기준 회전
	float yaw = 0.0f;		// Z축 기준 회전

	float theta;
	float phi;
	float chi;

	// speed (deltaTime/s)
	float deltaRoll;
	float deltaPitch;
	float deltaYaw;
	
	float deltaTheta;
	float deltaPhi;
	float deltaChi;

	DirectX::XMFLOAT3X3 transformMatrix;
};

