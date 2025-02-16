struct VertexOut
{
    float2 textureCoord : TEXCOORD;
    float4 position : SV_Position;
};

VertexOut VS(float2 position : Position)
{
    VertexOut vertexOut;
    vertexOut.position = float4(position, 0.0f, 1.0f);
    vertexOut.textureCoord = float2((position.x + 1) / 2.0f, -(position.y - 1) / 2.0f);
    
    return vertexOut;
}

Texture2D tex;
SamplerState state;

cbuffer Kernal
{
    uint length;
    float sampleCoefficients[15];
};

cbuffer Control
{
    bool isHorizontal;
};

float4 PS(float2 textureCoord : TEXCOORD) : SV_Target
{
    float width, height;
    tex.GetDimensions(width, height);
    
    float dx, dy;
    
    if (isHorizontal)
    {
        dx = 1.0f / width;
        dy = 0.0f;
    }
    
    else
    {
        dx = 0.0f;
        dy = 1.0f / height;
    }
    
    const int radius = length / 2;
    
    float3 maxColor = float3(0.0f, 0.0f, 0.0f);
    float accAlpha = 0.0f;
    
    for (int i = -radius; i <= radius; i++)
    {
        const float2 tc = textureCoord + float2(dx * i, dy * i);
        const float4 samplerTexture = tex.Sample(state, tc).rgba;
        const float sampleCoeff = sampleCoefficients[i + radius];
        
        accAlpha += samplerTexture.a * sampleCoeff;
        maxColor = max(samplerTexture.rgb, maxColor);
    }
    
    return float4(maxColor, accAlpha);
}