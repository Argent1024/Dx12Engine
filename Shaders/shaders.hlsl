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
	float3 diffuseColor;
}

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

SamplerState g_sampler : register(s0);

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	
	result.position = input.position;
	result.position = mul(result.position, modelTransformation);
	result.position = mul(result.position, view);
	result.position = mul(result.position, projection);

	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	
	// return float4(1.0, 0.0, 0.0, 0.0);
	
	float4 lightDir = SceneLights[0].strength;
	return lightDir;
	//return dot(lightDir.xyz, input.normal); //float4(input.normal, 1.0);
}