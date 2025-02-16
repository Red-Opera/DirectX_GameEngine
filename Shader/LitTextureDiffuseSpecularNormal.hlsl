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
    float2 textureCoord : TEXCOORD;
    float4 position : SV_Position;
}; 

cbuffer ObjectColor
{
    bool useGlassAlpha;     // Alpha 사용 여부
    bool useSpecularMap;    // Specular 사용 여부
    float3 specularColor;   // Specular 색깔
    float specularPower;    // Specular 강도
    float specularGlass;    // Specular 반짝임 강도
    bool useNormalMap;      // Normal 사용 여부
    float normalMapPower;   // Normal 맵 강도
};

Texture2D tex;
Texture2D specularTexture;
Texture2D normalMap;

SamplerState state;

VertexOut VS(float3 localPosition : Position, float3 normal : Normal, float3 tangent : Tangent, float3 biTangent : Bitangent, float2 textureCoord : TEXCOORD)
{
    VertexOut vertexOut;
    
    vertexOut.viewPosition = (float3) mul(float4(localPosition, 1.0f), cameraTransform);    // 카메라 위치에서 viewPosition을 구함
    vertexOut.normal = mul(normal, (float3x3) cameraTransform);                             // 해당 위치에서 Normal을 구함
    vertexOut.tangent = mul(tangent, (float3x3) cameraTransform);                           // 카메라 위치에서 모델의 Tangent를 구함
    vertexOut.biTangent = mul(biTangent, (float3x3) cameraTransform);                       // 카메라 위치에서 모델의 Bitangent를 구함
    vertexOut.position = mul(float4(localPosition, 1.0f), worldViewProjection);             // 화면 상의 위치를 구함
    
    vertexOut.textureCoord = textureCoord;
    
    return vertexOut;
}

float4 PS(float3 viewPosition : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBiTangent : Bitangent, float2 textureCoord : TEXCOORD) : SV_Target
{
    float4 sampleTexture = tex.Sample(state, textureCoord); // 텍스쳐의 색을 가져옴
    
    // 알파를 사용하는 경우
#ifdef USE_ALPHA_MASK
    // Alpha가 0.1보다 작으면 제거
    clip(sampleTexture.a < 0.1f ? -1 : 1);                  
    
    // 해당 픽셀의 노멀 값이 1이 넘지 않고 방향만 가리킬 수 있도록 설정
    if (dot(viewNormal, viewPosition) >= 0.0f)
        viewNormal = -viewNormal;
#endif
    
    viewNormal = normalize(viewNormal);
    
    if (useNormalMap)
    {
        const float3 mappedNormal = GetViewNormal(normalize(viewTangent), normalize(viewBiTangent), viewNormal, textureCoord, normalMap, state);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapPower);
    }
    
    // 정점에서 빛의 거리와 방향을 구함
    const LightVector lightVector = GetLightVector(lightViewPosition, viewPosition);

    float3 specularReflectionColor;             // Speular가 반사하는 색깔
    float specularPower = specularGlass;
    const float4 specularSample = specularTexture.Sample(state, textureCoord);
    
    // Specular Map을 사용할 경우
    if (useSpecularMap)
        specularReflectionColor = specularSample.rgb;
    
    else
        specularReflectionColor = specularColor;
    
    // 반짝임을 사용하는 경우
    if (useGlassAlpha)
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    
    // 상쇠된 빛의 강도를 구하고 분산광을 거리에 따른 빛의 강도를 구함
    const float attResult = GetAttenuate(attConst, attLin, attQuad, lightVector.distance);
    const float3 diffuse = GetDiffuse(diffuseColor, diffuseIntensity, attResult, lightVector.vertexToLightDir, viewNormal);
    
    // 반사광의 specular를 구함
    const float3 specularRefeflect = GetSpecular(diffuseColor * diffuseIntensity * specularReflectionColor, specularPower, viewNormal, lightVector.vertexToLight, viewPosition, attResult, specularPower);
    
    // 주변광과 분산광을 합친 후 1.0이 넘어간 경우 최대 1.0으로 제한함
    return float4(saturate((diffuse + ambient) * sampleTexture.rgb + specularRefeflect), 1.0f);
}