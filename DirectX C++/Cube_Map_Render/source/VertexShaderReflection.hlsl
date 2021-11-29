cbuffer MatrixBuffer : register(b0)
{
    matrix ModelToWorldMatrix;
    matrix WorldToViewMatrix;
    matrix ProjectionMatrix;
};

struct VSIn
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEX;
};

struct PSIn
{
    float4 pos : SV_Position;
    float3 worldPos : WORLDPOS;
    float3 normal : NORMAL;
};

PSIn main(VSIn input)
{
    PSIn psin;

    matrix MV = mul(WorldToViewMatrix, ModelToWorldMatrix);
	// model-to-projection
    matrix MVP = mul(ProjectionMatrix, MV);
	
    psin.pos = mul(MVP, float4(input.Pos, 1));
    psin.worldPos = mul(ModelToWorldMatrix, float4(input.Pos, 1)).xyz;
    psin.normal = mul(ModelToWorldMatrix, input.Normal);

    return psin;
}