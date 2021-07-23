#define EPLISION 0.00005f

#define PI 3.141592653589793f
#define TwoPI 6.283185307179586f

// Store Camera Transformations and other settings
cbuffer SceneInfo : register(b0) 
{
	// Camera transformation
	float4x4 projection;
	float4x4 view;
	float3 CameraPos;
};


cbuffer ObjectInfo : register(b2)
{
	float4x4 modelTransformation;
}

TextureCube EnvMapping : register(t0, space1);
SamplerState g_sampler : register(s0);

struct VSInput 
{
	float3 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 uv		: TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 uv		: TEXCOORD0;
};

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	
	float4 pos = float4(input.position, 1.0f);
	pos = mul(pos, modelTransformation);	
	pos = mul(pos, view);
	pos = mul(pos, projection);

	result.position = pos;
    result.uv = input.position;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 uv = normalize(input.uv);
	float3 baseCol = EnvMapping.SampleLevel(g_sampler, uv, 1).xyz;
	return float4(baseCol, 1.0); //float4((uv + 1.0) / 2.0, 1.0);
}