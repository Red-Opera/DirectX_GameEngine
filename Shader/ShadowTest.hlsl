#include "ShaderHeader/PointLight.hlsli"
#include "ShaderHeader/LightShader.hlsli"
#include "ShaderHeader/LightVector.hlsli"
#include "ShaderHeader/Transform.hlsli"
#include "ShaderHeader/ShadowVS.hlsli"
#include "ShaderHeader/ShadowStaticPS.hlsli"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 viewNormal : Normal;
    float2 textureCoord : TEXCOORD;
    float4 shadowPosition : ShadowPosition;
    float4 position : SV_Position;
};

cbuffer ObjectColor : register(b1)
{
    float3 specularColor;
    float specularPower; // Specular 강도
    float specularGlass;
};

Texture2D tex : register(t0);
SamplerState state : register(s0);

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform); // 카메라 위치에서 viewPosition을 구함
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform); // 해당 위치에서 Normal을 구함
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection); // 화면 상의 위치를 구함
    
    vertexOut.textureCoord = textureCoord;
    
    vertexOut.shadowPosition = GetShadowHomoSpace(localPosition, model);
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float2 textureCoord : TEXCOORD, float4 shadowPosition : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    const float shadow = GetShadow(shadowPosition);
    if (shadow != 0.0f)
    {
        // 정점에서 빛의 거리와 방향을 구함
        const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
        // 상쇠된 빛의 강도를 구하고 분산광을 거리에 따른 빛의 강도를 구함
        const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
        diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
        specular = GetSpecular(diffuseColor * diffuseIntensity * specularColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularGlass);
        
        diffuse *= shadow;
        specular *= shadow;
    }
    
    else
        diffuse = specular = 0.0f;
    
    // 주변광과 분산광을 합친 후 1.0이 넘어간 경우 최대 1.0으로 제한함
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specular), 1.0f);
}