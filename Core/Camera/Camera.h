#pragma once

#include "CameraProjection.h"
#include "CameraIndicator.h"

#include "Core/Component/Component.h"

#include <DirectXMath.h>
#include <string>

namespace RenderGraphNameSpace { class RenderGraph; }

class DxGraphic;

class Camera : public Component
{
public:
    Camera
    (
        std::shared_ptr<class Object> object,
        bool isTethered = false
    ) noexcept;

    DirectX::XMMATRIX GetMatrix() const noexcept;			// 카메라의 Matrix를 반환함
    DirectX::XMMATRIX GetProjection() const noexcept;
    
    void SpawnControlWidgets() noexcept;					// 카메라를 조절할 수 있는 ImGui를 생성함
    void Reset() noexcept;									// 카메라 위치 초기화
    
    void Rotate(float dx, float dy) noexcept;
    void Translate(Position translation) noexcept;

    void SetPosition(const Position& position) noexcept;
    Position& GetPosition() const noexcept;					// 카메라 위치를 반환함
    const std::string GetName() const noexcept;

    void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);
    void Submit(size_t channel) const;

    void RenderToGraphic() const;

    ~Camera() override = default;

    void Initialize() override;
    void Update() override;
    void LateUpdate() override;

    virtual std::string GetClassName() const override { return "CameraComponent"; }
    static std::string GetStaticClassName() { return "CameraComponent"; }

private:
    CameraIndicator indicator;
    CameraProjection projection;

    static constexpr float moveSpeed = 12.0f;		// 카메라 이동 속도
    static constexpr float rotationSpeed = 0.0004f;	// 카메라 회전 속도

    bool isEnableIndicator = false;
    bool isEnableFrustumIndicator = false;
    bool isTethered;
};