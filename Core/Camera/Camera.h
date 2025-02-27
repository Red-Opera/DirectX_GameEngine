#pragma once

#include "CameraProjection.h"
#include "CameraIndicator.h"

#include <DirectXMath.h>
#include <string>

namespace RenderGraphNameSpace { class RenderGraph; }

class DxGraphic;

class Camera
{
public:
	Camera(DxGraphic& graphic, std::string name, DirectX::XMFLOAT3 initPosition = { -22.0f, 4.0f, 0.0f }, float initPitch = 0.0f, float initYaw = 1.57f, bool isTethered = false) noexcept;

	DirectX::XMMATRIX GetMatrix() const noexcept;			// 카메라의 Matrix를 반환함
	DirectX::XMMATRIX GetProjection() const noexcept;
	
	void SpawnControlWidgets(DxGraphic& graphic) noexcept;	// 카메라를 조절할 수 있는 ImGui를 생성함
	void Reset(DxGraphic& graphic) noexcept;				// 카메라 위치 초기화
	
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
	DirectX::XMFLOAT3 GetPosition() const noexcept;			// 카메라 위치를 반환함
	const std::string& GetName() const noexcept;

	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);
	void Submit(size_t channel) const;

	void RenderToGraphic(DxGraphic& graphic) const;

private:
	std::string name;

	CameraIndicator indicator;
	CameraProjection projection;

	DirectX::XMFLOAT3 position;	// 카메라 위치
	DirectX::XMFLOAT3 initPosition;

	// 카메라 회전
	float pitch = 0.0f;
	float yaw = 0.0f;

	float initPitch;
	float initYaw;

	static constexpr float moveSpeed = 12.0f;		// 카메라 이동 속도
	static constexpr float rotationSpeed = 0.0004f;	// 카메라 회전 속도

	bool isEnableIndicator = false;
	bool isEnableFrustumIndicator = false;
	bool isTethered;
};