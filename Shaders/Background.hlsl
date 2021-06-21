#define EPLISION 0.00005f

struct Light  // Equal to LightState Class
{
	float4 strength;
	float4 pos;			// point / spot light
	float4 direction;	// dir / spot light
	float4x4 view;		// transform from world to light
	float4x4 proj;		// store projective(point spot) / orthnormal(dir) matrix
};

// Store Camera Transformations and other settings
cbuffer SceneInfo : register(b0) 
{
	// Camera transformation
	float4x4 projection;
	float4x4 view;
	float3 CameraPos;

	// Debug settings
	bool debugnormal;
	bool debugpos;
};

//const uint maxSceneLight = 16;
cbuffer LightInfo : register(b1)
{
	Light SceneLights[16];
	uint maxDir;
	uint maxPoint;
	uint maxSpot;
}


cbuffer ObjectInfo : register(b2)
{
	float4x4 modelTransformation;
}

Texture2D EnvMapBaseColor : register(t0);
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
    float2 uv		: TEXCOORD0;
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
    result.uv = input.uv;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //return float4(1.0, 0.0, 0.0, 1.0);
    float2 uv = input.uv;
    uv.x = frac(uv.x - 0.3);
	float3 baseCol = 1.3 * EnvMapBaseColor.Sample(g_sampler, uv).xyz;
	return float4(baseCol, 1.0);
}