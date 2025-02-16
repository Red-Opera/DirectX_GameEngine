// WolrdViewProjection�� ��� ����
// ��ü���� �����ؾ��ϱ� ������ ���� �󵵰� ���� ��� ����
cbuffer ObjectWorldViewProjection
{
    float4x4 worldViewProjection;
};

// ������ �����϶� �����ϱ� ������ ���� �󵵰� ������ ��� ����
cbuffer LightBuffer
{
    float3 lightDirection;
    float3 lightPosition;
    float3 lightColor;
};

// ���� �󵵰� ���� ���� ��� ����
cbuffer FogBuffer
{
    float4 fogColor;
    float fogStart;
    float fogEnd;
};

void ColorVertexShader(float3 position : POSITION, float4 color : COLOR, 
                  out float4 returnPosition : SV_Position, out float4 returnColor : COLOR)
{
    // ������ ���� ������ ���� ���� �������� �̵���
    returnPosition = mul(float4(position, 1.0f), worldViewProjection);
    
    // ���� ���� �׷��� ���� �ܰ�� ����
    returnColor = color;
}

float4 ColorPixelShader(float4 position : SV_Position, out float4 color : COLOR) : SV_Target
{
    // ������ ��ȯ�� ����� �Ѵ�.
    return color;
}

// Rasterizer�� ���� ������ ����
RasterizerState WireFrameRS
{
    FillMode = WireFrame;
    CullMode = Back;
    FrontCounterClockwise = false;
};

// ���� ������ ȿ���� ó���ϱ� ���� ȿ�� �����ӿ�ũ
technique11 ColorTech
{
    pass P0
    {
        //SetVertexShader(CompileShader(vs_5_0, ColorVertexShader()));
        //SetGeometryShader(NULL);
        //SetPixelShader(CompileShader(ps_5_0, ColorPixelShader()));

        VertexShader = compile vs_5_0 ColorVertexShader();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 ColorPixelShader();

        // Rasterizer ���� ������ ����
        SetRasterizerState(WireFrameRS);
    }
}