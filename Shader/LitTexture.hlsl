#include "ShaderHeader/PointLight.hlsl"
#include "ShaderHeader/LightShader.hlsl"
#include "ShaderHeader/LightVector.hlsl"
#include "ShaderHeader/Transform.hlsl"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 viewNormal : Normal;
    float4 position : SV_Position;
};

cbuffer ObjectColor
{
    float3 materialColor;       // ��ü�� ���� ���� 
    float3 specularColor;       // Specular ����
    float specularPower;        // Specular ����
    float specularGlass;        // ��¦�� ����
};

VertexOut VS(float3 localPosition : Position, float3 normal : Normal)
{
    VertexOut vertexOut;
    
  //vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), transform);     // localPosition���� worldPosition�� ����
  //vertexOut.normal = mul(normal, (float3x3) transform);                               // �ش� ��ġ���� Normal�� ����
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform); // ī�޶� ��ġ���� worldPosition�� ����
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform);                         // �ش� ��ġ���� Normal�� ����
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
    const float3 specular = GetSpecular(diffuseColor * diffuseIntensity * specularColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularGlass);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}