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

/********************** Object & Material Descriptor Table Start ***********************/

// Store Object information
//		Transformation
//		Materials' data
cbuffer ObjectInfo : register(b2)
{
	float4x4 modelTransformation;
}

cbuffer MaterialInfo : register (b3) 
{
	uint TextureResolution;
}

Texture2D<float4> DisplacementTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
SamplerState g_sampler : register(s0);



/********************** Object & Material Descriptor Table End ****************************/


struct VSInput 
{
	float3 position : POSITION0;
	uint2 uv 		: TEXCOORD0;
	float2 fuv		: TEXCOORD1;
};


struct PSInput
{
    float4 position : SV_POSITION;
	float4 worldpos : POSITION0;
	float4 camerapos: POSITION1;
	float4 screenpos: POSITION2;
	float3 normal   : COLOR0;

	float2 uv		: TEXCOORD0;
	
};

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	
	float3 shift = DisplacementTex[input.uv].xyz / 20.0;
	float4 pos = float4(input.position + shift, 1.0f);

	pos = mul(pos, modelTransformation);
	result.worldpos = pos;
	
	pos = mul(pos, view);
	result.camerapos = pos;

	pos = mul(pos, projection);

	result.position = pos;
	result.screenpos = pos;

	result.uv = input.fuv;
    return result;
}


// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	
	float2 uv = float2(input.uv);
	float shift = 0.5;
	//float3 ans = DisplacementTex.Sample(g_sampler, uv).xyz + shift;
	float3 ans = NormalTex.Sample(g_sampler, uv).xyz + shift;
	return float4(ans, 1.0);
}