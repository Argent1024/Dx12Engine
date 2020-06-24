#define SHADOW_DEPTH_BIAS 0.00005f

struct Light 
{
	float3 strength;
	float3 pos;			// point / spot light
	float3 direction;	// dir / spot light
	float4x4 view;		// transform from world to light
	float4x4 proj;		// store projective(point spot) / orthnormal(dir) matrix
};

// Store Camera Transformation and Lights' data
cbuffer CameraInfo : register(b0) 
{
	// Camera transformation
	float4x4 projection; 
	float4x4 view;
};

// Store Object information
//		Transformation
//		Materials' data
cbuffer ObjectInfo : register(b2)
{
	float4x4 modelTransformation;
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
// Texture2D objTexture : register(t0);
// Texture2D depthTexture : register(t1);


// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	float4x4 mvp = modelTransformation * view * projection;
    result.position = mul(mvp, input.position);
	//result.position = input.position;
	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	float4 lightSpacePos = mul(input.position, dirLight.view);
	lightSpacePos = mul(lightSpacePos, dirLight.orthnormal);

	float2 shadowTexUV = 0.5f * lightSpacePos.xy + 0.5f;
	shadowTexUV.y = 1 - shadowTexUV.y;

	float shadowDepth = depthTexture.Sample(g_sampler, shadowTexUV);	// length to the nearest point where light reach
	float lightDepth = lightSpacePos.z - SHADOW_DEPTH_BIAS;				// length from light to this point

	float shadowTest = (lightDepth <= shadowDepth) ? 1.0f : 0.0f;

	float cosAngle = dot(input.normal, dirLight.direction);
	
	float diffuseColor = 0.9;

	return float4(diffuseColor * cosAngle * shadowTest * dirLight.radiance, 1);
}