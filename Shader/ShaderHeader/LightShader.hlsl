// 해당 픽셀에 해당하는 Local Normal 값을 View Noraml 값으로 변경시키는 함수
float3 GetViewNormal(const in float3 tangent, const in float3 biTangent, const in float3 normal,
                     const in float2 textureCoord, uniform Texture2D normalMap, uniform SamplerState state)
{
    // Tangent, biTangent, Normal 값을 하나의 행렬로 표현
    // TangentX,    TangentY,   TangetZ
    // biTangentX,  biTangentY, biTangentZ
    // NormalX,     NormalY,    NormalZ
    const float3x3 tangetToView = float3x3(tangent, biTangent, normal);
        
    // 범위를 [0, 1] -> [-1, 1]
    const float3 normalSample = normalMap.Sample(state, textureCoord).xyz;
    const float3 tangentNormal = normalSample * 2.0f - 1.0f;
        
    // 접선 공간(Tangent Space)에서 뷰 공간(View Speace)로 viewNormal을 변경함
    return normalize(mul(tangentNormal, tangetToView));
}

// 거리에 따른 상쇠된 빛 세기를 구하는 함수
float GetAttenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distance)
{
    return 1.0f / (attConst + attLin * distance + attQuad * (distance * distance));
}

// 거리에 따른 분산광 세기를 구하는 함수
float3 GetDiffuse(uniform float3 diffuseColor, uniform float diffuseIntensity, const in float attResult, const in float3 vertexToLightDir, const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * attResult * max(0.0f, dot(vertexToLightDir, viewNormal));
}

float3 GetSpecular(
    const in float3 diffuseColor, uniform float diffuseIntensity,
    const in float3 viewNormal, const in float3 vertexToLight, const in float3 viewPosition, const in float attResult, const in float specularPower)
{
    // 빛이 반사되는 방향을 구함 
    const float3 reflectAngle = viewNormal * dot(vertexToLight, viewNormal); // 노말 벡터와 오브젝트에서 빛의 각도를 구함
    const float3 reflectVector = normalize(reflectAngle * 2.0f - vertexToLight); // 빛이 반사되는 각도를 구함
    
    // 반사광 법칙으로 반사광을 구함
    return attResult * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-reflectVector), normalize(viewPosition))), specularPower);
}