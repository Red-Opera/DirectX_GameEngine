#include "stdafx.h"
#include "TextureCube.h"
#include "Cube.h"

#include "Base/Image/Texture.h"
#include "Base/Material.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"

TextureCube::TextureCube(DxGraphic& graphic, std::mt19937& random, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
    : TestObject(graphic, random, adist, ddist, odist, rdist)
{
    if (!IsStaticInitialized())
    {
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 normal;
            DirectX::XMFLOAT2 textureCoord;
        };

        auto instance = Cube::MakeIndependentTextured<Vertex>();
        instance.SetNormalVector();

        AddStaticBind(std::make_unique<VertexBuffer>(graphic, instance.vertices));
        AddStaticBind(std::make_unique<Texture>(graphic, Material::FromFile("Images\\images.png")));
        AddStaticBind(std::make_unique<SamplerState>(graphic));

        auto vertexShader = std::make_unique<VertexShader>(graphic, L"Shader/TextureLitShader.hlsl");
        auto shaderCode = vertexShader->GetShaderCode();
        AddStaticBind(std::move(vertexShader));

        AddStaticBind(std::make_unique<PixelShader>(graphic, L"Shader/TextureLitShader.hlsl"));
        AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphic, instance.indices));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        AddStaticBind(std::make_unique<InputLayout>(graphic, inputLayout, shaderCode));
        AddStaticBind(std::make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

        struct PSMaterialConstant
        {
            float specularIntensity = 0.6f;
            float specularPower = 30.0f;
            float padding[2];
        } colorConst;

        AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphic, colorConst, 1));
    }

    else
        SetIndexFromStatic();

    AddBind(std::make_unique<TransformConstantBuffer>(graphic, *this));
}