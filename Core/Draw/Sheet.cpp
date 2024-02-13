#include "stdafx.h"
#include "Sheet.h"
#include "Plane.h"

#include "Base/Material.h"
#include "Base/Image/Texture.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"

Sheet::Sheet(DxGraphic& graphic, std::mt19937& random, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
    : r(rdist(random)), deltaRoll(ddist(random)), deltaPitch(ddist(random)), deltaYaw(ddist(random)),
      deltaPhi(odist(random)), deltaTheta(odist(random)), deltaChi(odist(random)),
      chi(adist(random)), theta(adist(random)), phi(adist(random))
{
    if (!IsStaticInitialized())
    {
        struct Vertex
        {
            DirectX::XMFLOAT3 position;

            struct { float u, v; } texCoord;
        };

        auto instance = Plane::Make<Vertex>();
        instance.vertices[0].texCoord = { 0.0f, 0.0f };
        instance.vertices[1].texCoord = { 1.0f, 0.0f };
        instance.vertices[2].texCoord = { 0.0f, 1.0f };
        instance.vertices[3].texCoord = { 1.0f, 1.0f };

        AddStaticBind(std::make_unique<Texture>(graphic, Material::FromFile("Images/Images.png")));
        AddStaticBind(std::make_unique<VertexBuffer>(graphic, instance.vertices));
        AddStaticBind(std::make_unique<SamplerState>(graphic));

        auto vertexShader = std::make_unique<VertexShader>(graphic, L"Shader/Texture.hlsl");
        auto shaderCode = vertexShader->GetShaderCode();
        AddStaticBind(std::move(vertexShader));

        AddStaticBind(std::make_unique<PixelShader>(graphic, L"Shader/Texture.hlsl"));
        AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphic, instance.indices));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };


        AddStaticBind(std::make_unique<InputLayout>(graphic, inputLayout, shaderCode));
        AddStaticBind(std::make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }

    else
        SetIndexFromStatic();

    AddBind(std::make_unique<TransformConstantBuffer>(graphic, *this));
}

void Sheet::Update(float dt) noexcept
{
    roll += deltaRoll * dt;
    pitch += deltaPitch * dt;
    yaw += deltaYaw * dt;
    theta += deltaTheta * dt;
    phi += deltaPhi * dt;
    chi += deltaChi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
    namespace dx = DirectX;
    return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
        dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}