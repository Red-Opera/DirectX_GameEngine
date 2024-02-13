// 해당 물체의 Transfrom을 저장하기위한 cbuffer
cbuffer TransformBuffer
{
    matrix transform;
};

struct Vertex
{
    float2 textCoord : TexCoord;        // 텍스처 좌표 위치
    float4 position : SV_Position;
};

// Texture2D  texgture : register(t0)에서 생략할 수 있음
Texture2D inputTexture;          // 텍스처를 입힐 텍스처
SamplerState samplerState;  // Sampler State에서 설정한 값

Vertex VS(float3 position : Position, float2 textCoord : TexCoord)
{
    Vertex vertex;
    vertex.position = mul(float4(position, 1.0f), transform);
    vertex.textCoord = textCoord;                               // 정점의 텍스처 좌표를 입력함
    
    return vertex;
}

float4 PS(float2 textCoord : TexCoord) : SV_Target
{
    // 해당 텍스처에서 Sampler State에 설정한 결과로 봔환함
    return inputTexture.Sample(samplerState, textCoord);
}