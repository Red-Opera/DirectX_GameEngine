#pragma once

#include "Core/Component/Component.h"
#include "Core/Draw/Base/Drawable.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"
#include "Core/Draw/Base/Image/Image.h"

#include "Utility/Vector.h"

class ColorObject : public Drawable, public Component
{
public:
    ColorObject(std::shared_ptr<class Object> object);

    void SetRenderingPipeline(GraphicResource::Image::Color color, bool isLit, class TriangleIndexList model);

    DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

    void SetColor(GraphicResource::Image::Color color);
    GraphicResource::Image::Color GetColor() const;

    void SetLit(bool isLit);
    bool GetLit() const;

    void Initialize() override;
    void Update(float deltaTime) override;
    void LateUpdate() override;

    virtual std::string GetClassName() const override { return "ColorObject"; }
    static std::string GetStaticClassName() { return "ColorObject"; }

protected:
    GraphicResource::Image::Color color = { 255, 255, 255 };

    bool isLit = true;
};