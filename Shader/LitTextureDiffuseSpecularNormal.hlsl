#include "ShaderHeader/PointLight.hlsli"
#include "ShaderHeader/LightShader.hlsli"
#include "ShaderHeader/LightVector.hlsli"
#include "ShaderHeader/Transform.hlsli"
#include "ShaderHeader/ShadowVS.hlsli"
#include "ShaderHeader/ShadowStaticPS.hlsli"

struct VertexOut
{
    float3 viewPosition : Position;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 biTangent : Bitangent;
    float2 textureCoord : TEXCOORD;
    float4 shadowHomoPosition : ShadowPosition;
    float4 position : SV_Position;
}; 

cbuffer ObjectColor : register(b1)
{
    bool useGlassAlpha;     // Alpha ��� ����
    bool useSpecularMap;    // Specular ��� ����
    float3 specularColor;   // Specular ����
    float specularPower;    // Specular ����
    float specularGlass;    // Specular ��¦�� ����
    bool useNormalMap;      // Normal ��� ����
    float normalMapPower;   // Normal �� ����
};

Texture2D tex : register(t0);
Texture2D specularTexture : register(t1);
Texture2D normalMap : register(t2);

SamplerState state : register(s0);

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float3 tangent : Tangent, float3 biTangent : Bitangent, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // ī�޶� ��ġ���� viewPosition�� ����
    vertexOut.normal = mul(normal, (float3x3) cameraTransform);                             // �ش� ��ġ���� Normal�� ����
    vertexOut.tangent = mul(tangent, (float3x3) cameraTransform);                           // ī�޶� ��ġ���� ���� Tangent�� ����
    vertexOut.biTangent = mul(biTangent, (float3x3) cameraTransform);                       // ī�޶� ��ġ���� ���� Bitangent�� ����
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // ȭ�� ���� ��ġ�� ����
    
    vertexOut.textureCoord = textureCoord;
    
    vertexOut.shadowHomoPosition = GetShadowHomoSpace(localPosition, model);
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBiTangent : Bitangent, float2 textureCoord : TEXCOORD, float4 shadowPosition : ShadowPosition) : SV_Target
{
    
    float3 diffuse;
    float3 specularRefeflect;
    
    float4 sampleTexture = tex.Sample(state, textureCoord); // �ؽ����� ���� ������
    
    // ���ĸ� ����ϴ� ���
#ifdef USE_ALPHA_MASK
    // Alpha�� 0.1���� ������ ����
    clip(sampleTexture.a < 0.1f ? -1 : 1);                  
    
    // �ش� �ȼ��� ��� ���� 1�� ���� �ʰ� ���⸸ ����ų �� �ֵ��� ����
    if (dot(viewNormal, viewPosition) >= 0.0f)
        viewNormal = -viewNormal;
#endif
    
    const float shadow = GetShadow(shadowPosition);
    if (shadow != 0.0f)
    {
        viewNormal = normalize(viewNormal);
        
        if (useNormalMap)
        {
            const float3 mappedNormal = GetViewNormal(normalize(viewTangent), normalize(viewBiTangent), viewNormal, textureCoord, normalMap, state);
            viewNormal = lerp(viewNormal, mappedNormal, normalMapPower);
        }
        
        // �������� ���� �Ÿ��� ������ ����
        const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);

        float3 specularReflectionColor; // Speular�� �ݻ��ϴ� ����
        float specularPower = specularGlass;
        const float4 specularSample = specularTexture.Sample(state, textureCoord);
        
        // Specular Map�� ����� ���
        if (useSpecularMap)
            specularReflectionColor = specularSample.rgb;
        else
            specularReflectionColor = specularColor;
        
        // ��¦���� ����ϴ� ���
        if (useGlassAlpha)
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        
        // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
        const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
        diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
        
        // �ݻ籤�� specular�� ����
        specularRefeflect = GetSpecular(diffuseColor * diffuseIntensity * specularReflectionColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
        diffuse *= shadow;
        specularRefeflect *= shadow;
    }
    
    else
        diffuse = specularRefeflect = 0.0f;
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate((diffuse + ambient) * sampleTexture.rgb + specularRefeflect), 1.0f);
}