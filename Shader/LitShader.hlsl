#include "ShaderHeader/PointLight.hlsl"
#include "ShaderHeader/LightShader.hlsl"
#include "ShaderHeader/LightVector.hlsl"
#include "ShaderHeader/Transform.hlsl"

struct VertexOut
{
    float3 worldPosition : Position;
    float3 viewNormal : Normal;
    float4 position : SV_Position;
};

cbuffer ObjectColor
{
    float4 materialColor;       // ��ü�� ���� ����
    float4 specularColor;       // Specular ����
    float specularPower;        // Specular ����
};

VertexOut VS(float3 localPosition : Position, float3 normal : Normal)
{
    VertexOut vertexOut;
    
  //vertexOut.worldPosition = (float3) mul(float4(localPosition, 1.0f), transform);     // localPosition���� worldPosition�� ����
  //vertexOut.normal = mul(normal, (float3x3) transform);                               // �ش� ��ġ���� Normal�� ����
    
    vertexOut.worldPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);   // ī�޶� ��ġ���� worldPosition�� ����
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform);                             // �ش� ��ġ���� Normal�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // ȭ�� ���� ��ġ�� ����
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal) : SV_Target
{
    // �ش� �ȼ��� ��� ���� 1�� ���� �ʰ� ���⸸ ����ų �� �ֵ��� ����
    viewNormal = normalize(viewNormal);
    
    // �������� ���� �Ÿ��� ������ ����
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
    // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    // �ݻ籤�� specular�� ����
    const float3 specular = GetSpecular(specularColor.rgb, 1.0f, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * materialColor.rbg + specular), 1.0f);
}