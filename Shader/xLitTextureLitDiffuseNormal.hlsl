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
    float specularIntensity;    // Specular ����
    float specularPower;        // Specular ����
    bool normalMapEnabled;      // Normal ��� ����
    float padding[1];
};

Texture2D tex;
Texture2D normalMap : register(t2);
SamplerState state;

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform); // ī�޶� ��ġ���� viewPosition�� ����
    vertexOut.normal = mul(normal, (float3x3) cameraTransform); // �ش� ��ġ���� Normal�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection); // ȭ�� ���� ��ġ�� ����
    
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
    
    // �������� ���� �Ÿ��� ������ ����
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
    // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    // �ݻ籤 ��Ģ���� �ݻ籤�� ����
    const float3 specular = GetSpecular(specularIntensity, 1.0f, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specular), 1.0f);
}