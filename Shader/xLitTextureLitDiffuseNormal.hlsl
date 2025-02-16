#include "ShaderHeader/PointLight.hlsl"
#include "ShaderHeader/LightShader.hlsl"
#include "ShaderHeader/LightVector.hlsl"
#include "ShaderHeader/Transform.hlsl"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 normal : Normal;
    float2 textureCoord : TEXCOORD;
    float4 position : SV_Position;
};

cbuffer ObjectColor
{
    float specularIntensity;    // Specular 세기
    float specularPower;        // Specular 강도
    bool normalMapEnabled;      // Normal 사용 여부
    float padding[1];
};

Texture2D tex;
Texture2D normalMap : register(t2);
SamplerState state;

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform); // 카메라 위치에서 viewPosition을 구함
    vertexOut.normal = mul(normal, (float3x3) cameraTransform); // 해당 위치에서 Normal을 구함
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection); // 화면 상의 위치를 구함
    
    vertexOut.textureCoord = textureCoord;
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float2 textureCoord : TEXCOORD) : SV_Target
{
    if (normalMapEnabled)
    {
        const float3 normalSample = normalMap.Sample(state, textureCoord).xyz;
        
        float3 tangentNormal = normalSample * 2.0f - 1.0f;
        
        viewNormal = normalize(mul(tangentNormal, (float3x3) cameraTransform));
    }
    
    // 정점에서 빛의 거리와 방향을 구함
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
    // 상쇠된 빛의 강도를 구하고 분산광을 거리에 따른 빛의 강도를 구함
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    // 반사광 법칙으로 반사광을 구함
    const float3 specular = GetSpecular(specularIntensity, 1.0f, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
    // 주변광과 분산광을 합친 후 1.0이 넘어간 경우 최대 1.0으로 제한함
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specular), 1.0f);
}