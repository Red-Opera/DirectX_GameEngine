#include "ShaderHeader/PointLight.hlsl"
#include "ShaderHeader/LightShader.hlsl"
#include "ShaderHeader/LightVector.hlsl"
#include "ShaderHeader/Transform.hlsl"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 viewNormal : Normal;
    float2 textureCoord : TEXCOORD;
    float4 position : SV_Position;
};    

cbuffer ObjectColor
{
    float3 specularColor;   // Specular ����
    float specularPower;    // Specular ����
    float specularGlass;    // Specular ��¦�� ����
};

Texture2D tex;
SamplerState state;

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // ī�޶� ��ġ���� viewPosition�� ����
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform);                         // �ش� ��ġ���� Normal�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // ȭ�� ���� ��ġ�� ����
    
    vertexOut.textureCoord = textureCoord;
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float2 textureCoord : TEXCOORD) : SV_Target
{
    // �ش� �ȼ��� ��� ���� 1�� ���� �ʰ� ���⸸ ����ų �� �ֵ��� ����
    viewNormal = normalize(viewNormal);
    
    // �������� ���� �Ÿ��� ������ ����
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
    // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    const float3 specular = GetSpecular(diffuseColor * diffuseIntensity * specularColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularGlass);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specular), 1.0f);
}