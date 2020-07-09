struct Light  // Equal to LightState Class
{
	float4 strength;
	float4 pos;			// point / spot light
	float4 direction;	// dir / spot light
	float4x4 view;		// transform from world to light
	float4x4 proj;		// store projective(point spot) / orthnormal(dir) matrix
};

// Store Camera Transformations
cbuffer CameraInfo : register(b0) 
{
	// Camera transformation
	float4x4 projection;
	float4x4 view;
};

//const uint maxSceneLight = 16;
cbuffer LightInfo : register(b1)
{
	Light SceneLights[16];
	uint maxDir;
	uint maxPoint;
	uint maxSpot;
}

// Store Object information
//		Transformation
//		Materials' data
cbuffer ObjectInfo : register(b2)
{
	float4x4 modelTransformation;
}

cbuffer MaterialInfo : register (b3) 
{
	float4 diffuseColor;
}

struct VSInput 
{
	float3 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 uv		: TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
	float4 worldpos : POSITION0;
	float3 normal   : COLOR0;
    float2 uv		: TEXCOORD0;
};

SamplerState g_sampler : register(s0);

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	
	float4 pos = float4(input.position, 1.0f);
	pos = mul(pos, modelTransformation);

	result.worldpos = pos;
	
	pos = mul(pos, view);
	pos = mul(pos, projection);

	result.position = pos;
	
	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	//return input.position.z / 2;

	float3 lightDir = SceneLights[0].direction.xyz;
	float4 strength = SceneLights[0].strength;
	float3 normal = input.normal;

	float cos_ln = max(dot(lightDir, normal), 0);
	return cos_ln * strength * diffuseColor;
}