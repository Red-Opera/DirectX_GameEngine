#pragma once

#include "Core/Component/Component.h"
#include "Core/Exception/WindowException.h"
#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBuffer.h"

namespace RenderGraphNameSpace { class RenderGraph; }

class Camera;

class PointLight : public Component
{
public:
	PointLight
	(
		std::shared_ptr<class Object> object, 
		Position position = { 0.0f, 10.0f, 0.0f },
		float radius = 0.5f
	);

	void SpawnControlWidgets() noexcept;
	void Reset() noexcept;
	void Submit(size_t channel) const NOEXCEPTRELEASE;

	std::shared_ptr<Object> GetLightViewCamera() const noexcept;

	~PointLight() override = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;

	virtual std::string GetClassName() const override { return "PointLightComponent"; }
	static std::string GetStaticClassName() { return "PointLightComponent"; }

private:
	struct PointLightConstantBuffer
	{
		alignas(16) Position position;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};

	std::shared_ptr<Object> viewCamera;

	PointLightConstantBuffer lightInfo;
	PointLightConstantBuffer initLightInfo;

	mutable std::shared_ptr<class ColorSphereObject> mesh;
	mutable Graphic::PixelConstantBuffer<PointLightConstantBuffer> cBuffer;
};