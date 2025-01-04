#include "ShaderHeader/PointLight.hlsl"
#include "ShaderHeader/LightShader.hlsl"
#include "ShaderHeader/LightVector.hlsl"
#include "ShaderHeader/Transform.hlsl"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 biTangent : Bitangent;
    float2 textureCoord : Texcoord;
    float4 position : SV_Position;
}; 

cbuffer ObjectColor
{
    bool normalMapEnabled;      // Normal ��� ����
    bool hasSpecularMap;        // Specular ��� ����
    bool hasGlassMap;           // Alpha ��� ����
    float specularPowerCbuffer; // Specular ����
    
    float3 specularColor;       // Specular ����
    float specualrMapWeight;    // Specular �� ����
};

Texture2D tex;
Texture2D specularTexture;
Texture2D normalMap;

SamplerState state;

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float3 tangent : Tangent, float3 biTangent : Bitangent, float2 textureCoord : texCoord)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // ī�޶� ��ġ���� viewPosition�� ����
    vertexOut.normal = mul(normal, (float3x3) cameraTransform);                             // �ش� ��ġ���� Normal�� ����
    vertexOut.tangent = mul(tangent, (float3x3) cameraTransform);                           // ī�޶� ��ġ���� ���� Tangent�� ����
    vertexOut.biTangent = mul(biTangent, (float3x3) cameraTransform);                       // ī�޶� ��ġ���� ���� Bitangent�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // ȭ�� ���� ��ġ�� ����
    
    vertexOut.textureCoord = textureCoord;
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBiTangent : Bitangent, float2 textureCoord : TexCoord) : SV_Target
{
    float4 sampleTexture = tex.Sample(state, textureCoord); // �ؽ����� ���� ������
    
    // ���ĸ� ����ϴ� ���
#ifdef USE_ALPHA_MASK
    // Alpha�� 0.1���� ������ ����
    clip(sampleTexture.a < 0.1f ? -1 : 1);                  
    
    // �ش� �ȼ��� ��� ���� 1�� ���� �ʰ� ���⸸ ����ų �� �ֵ��� ����
    if (dot(viewNormal, viewPosition) >= 0.0f)
        viewNormal = -viewNormal;
#endif
    
    viewNormal = normalize(viewNormal);
    
    if (normalMapEnabled)
        viewNormal = GetViewNormal(normalize(viewTangent), normalize(viewBiTangent), viewNormal, textureCoord, normalMap, state);
    
    // �������� ���� �Ÿ��� ������ ����
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);

    float3 specularReflectionColor;             // Speular�� �ݻ��ϴ� ����
    float specularPower = specularPowerCbuffer;
    
    // Specular Map�� ����� ���
    if (hasSpecularMap)
    {
        const float4 specularSample = specularTexture.Sample(state, textureCoord);
        specularReflectionColor = specularSample.rgb * specualrMapWeight;
        
        if (hasGlassMap)
            specularPower = pow(2.0f, specularSample.a * 13);
    }
    
    // ���� ���� ����� ���
    else
        specularReflectionColor = specularColor;
    
    // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    // �ݻ籤�� specular�� ����
    const float3 specularRefeflect = GetSpecular(specularReflectionColor, 1.0f, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * sampleTexture.rgb + specularRefeflect), sampleTexture.a);
}