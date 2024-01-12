// WolrdViewProjection의 상수 버퍼
// 물체마다 갱신해야하기 때문에 갱신 빈도가 높은 상수 버퍼
cbuffer ObjectWorldViewProjection
{
    float4x4 worldViewProjection;
};

// 광원이 움직일때 갱신하기 때문에 갱신 빈도가 보통인 상수 버퍼
cbuffer LightBuffer
{
    float3 lightDirection;
    float3 lightPosition;
    float3 lightColor;
};

// 갱신 빈도가 거의 없는 상수 버퍼
cbuffer FogBuffer
{
    float4 fogColor;
    float fogStart;
    float fogEnd;
};

void VertexShader(float3 position : POSITION, float4 color : COLOR, 
                  out float4 returnPosition : SV_Position, out float4 returnColor : COLOR)
{
    // 정점의 국소 공간을 동차 절단 공간으로 이동함
    returnPosition = mul(float4(position, 1.0f), worldViewProjection);
    
    // 정점 색상 그래도 다음 단계로 보냄
    returnColor = color;
}

float4 PixelShder(float4 position : SV_Position, out float4 color : COLOR) : SV_Target
{
    // 무조건 반환을 해줘야 한다.
    return color;
}