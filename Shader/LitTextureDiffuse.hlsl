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
    float3 specularColor;   // Specular ����
    float specularPower;    // Specular ����
    float specularGlass;    // Specular ��¦�� ����
};

Texture2D tex : register(t0);
SamplerState state : register(s0);

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // ī�޶� ��ġ���� viewPosition�� ����
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform);                         // �ش� ��ġ���� Normal�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // ȭ�� ���� ��ġ�� ����
    
    vertexOut.textureCoord = textureCoord;
    
    vertexOut.shadowHomoPosition = GetShadowHomoSpace(localPosition, model);
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float2 textureCoord : TEXCOORD, float4 shadowPosition : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    const float shadow = GetShadow(shadowPosition);
    if (shadow != 0.0f)
    {
        // �ش� �ȼ��� ��� ���� 1�� ���� �ʰ� ���⸸ ����ų �� �ֵ��� ����
        viewNormal = normalize(viewNormal);
    
        // �������� ���� �Ÿ��� ������ ����
        const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
        // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
        const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
        diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
        specular = GetSpecular(diffuseColor * diffuseIntensity * specularColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularGlass);
        
        diffuse *= shadow;
        specular *= shadow;
    }
    
    else
        diffuse = specular = 0.0f;
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specular), 1.0f);
}