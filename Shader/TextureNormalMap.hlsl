#include "ShaderHeader/PointLight.hlsl"
#include "ShaderHeader/LightShader.hlsl"
#include "ShaderHeader/LightVector.hlsl"
#include "ShaderHeader/Transform.hlsl"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 viewNormal : Normal;
    float3 tangent : Tangent;
    float3 biTangent : Bitangent;
    float2 textureCoord : Texcoord;
    float4 position : SV_Position;
};     

cbuffer ObjectColor
{
    float specularIntensity;    // Specular ����
    float specularPower;        // Specular ����
    bool normalMapEnabled;     // Normal ��� ����
    float padding[1];
};

Texture2D tex;
Texture2D normalMap : register(t2);
SamplerState state;

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float3 tangent : Tangent, float3 biTangent : Bitangent,  float2 textureCoord : texCoord)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // ī�޶� ��ġ���� viewPosition�� ����
    vertexOut.viewNormal = mul(normal, (float3x3) cameraTransform);                         // �ش� ��ġ���� Normal�� ����
    vertexOut.tangent = mul(tangent, (float3x3) cameraTransform);                           // ī�޶� ��ġ���� ���� Tangent�� ����
    vertexOut.biTangent = mul(biTangent, (float3x3) cameraTransform);                       // ī�޶� ��ġ���� ���� Bitangent�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // ȭ�� ���� ��ġ�� ����
    
    vertexOut.textureCoord = textureCoord;
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBiTangent : Bitangent, float2 textureCoord : TexCoord) : SV_Target
{ 
    // �ش� �ȼ��� ��� ���� 1�� ���� �ʰ� ���⸸ ����ų �� �ֵ��� ����
    viewNormal = normalize(viewNormal);
    
    if (normalMapEnabled)
        viewNormal = GetViewNormal(normalize(viewTangent), normalize(viewBiTangent), viewNormal, textureCoord, normalMap, state);
    
    // �������� ���� �Ÿ��� ������ ����
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);
    
    // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    const float3 specular = GetSpecular(diffuseColor, diffuseIntensity, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * tex.Sample(state, textureCoord).rgb + specular), 1.0f);
}