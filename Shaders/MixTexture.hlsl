SamplerState g_sampler : register(s0);
Texture2D g_texture : register(t0);

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
	return 5 * (g_texture.Sample(g_sampler, input.uv).x - 0.8);
	// return float4(input.normal, 1);
}