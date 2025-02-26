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
    float4 shadowHomoPosition : ShadowPosition;
    float4 position : SV_Position;
};    

cbuffer ObjectColor : register(b1)
{
    bool useGlassAlpha;
    bool useSpecularMap;    // Specular 사용 여부
    float3 specularColor;
    float specularPower;   // Specular 강도
    float specularGlass;
};

Texture2D tex : register(t0);
Texture2D specularTexture : register(t1);

SamplerState state : register(s0);

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // 카메라 위치에서 viewPosition을 구함
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform);                         // 해당 위치에서 Normal을 구함
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // 화면 상의 위치를 구함
    
    vertexOut.textureCoord = textureCoord;
    
    vertexOut.shadowHomoPosition = GetShadowHomoSpace(localPosition, model);
    
    return vertexOut;
}


float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float2 textureCoord : TEXCOORD, float4 shadowPosition : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specularRefeflect;
    
    const float shadow = GetShadow(shadowPosition);
    if (shadow != 0.0f)
    {
        // 해당 픽셀의 노멀 값이 1이 넘지 않고 방향만 가리킬 수 있도록 설정
        viewNormal = normalize(viewNormal);
        
        // 정점에서 빛의 거리와 방향을 구함
        const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
        
        float specularPowerLoded = specularGlass;
        
        const float4 specularSample = specularTexture.Sample(state, textureCoord);
        
        // Speular가 반사하는 색깔
        float3 specularReflectionColor;
        
        // Specular Map을 사용할 경우
        if (useSpecularMap)
            specularReflectionColor = specularSample.rgb;
        else
            specularReflectionColor = specularColor;
        
        if (useGlassAlpha)
            specularPowerLoded = pow(2.0f, specularSample.a * 13.0f);
        
        // 상쇠된 빛의 강도를 구하고 분산광을 거리에 따른 빛의 강도를 구함
        const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
        diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
        
        // 반사광의 specular를 구함
        specularRefeflect = GetSpecular(diffuseColor * specularReflectionColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPowerLoded);
        
        diffuse *= shadow;
        specularRefeflect *= shadow;
    }
    
    else
        diffuse = specularRefeflect = 0.0f;
        
    // 주변광과 분산광을 합친 후 1.0이 넘어간 경우 최대 1.0으로 제한함
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specularRefeflect), 1.0f);
}