SamplerState g_sampler : register(s0);
Texture2D g_texture : register(t1);

// Store Camera Transformations
cbuffer TextureInfo : register(b2) 
{
	
};
 
struct VSInput 
{
	float4 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 uv		: TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
	float3 normal   : COLOR0;
    float2 uv		: TEXCOORD0;
};


// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	
    result.position = input.position;
	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}


float4 PSMain(PSInput input) : SV_TARGET
{
	// return float4(1.0, 0.0, 0.0, 0.0);
	float z = g_texture.Sample(g_sampler, input.uv).x;
	float c = 0.92f;
	return (z - c) / (1.0f - c);
	// return float4(input.normal, 1);
}