cbuffer SceneConstantBuffer : register(b0)
{
    float4 offset;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct VSInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

PSInput VSMain(VSInput v)
{
    PSInput result;

    result.position = v.position + offset;
    result.color = v.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float2 coor = input.color.xy;
    return coor.x;
}
