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
	
	float3 shift = DisplacementTex[input.uv].xyz / 100.0;
	// float3 shift = 0.0 ;
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
	const float PI = 3.14159265;
	float2 uv = float2(input.uv);
	float3 normal = normalize(NormalTex.Sample(g_sampler, uv).xyz);
	if (debugnormal) {
		// return (float4(normal, 1.0f) + 1.0f) / 2.0f;
		float3 shift = DisplacementTex.Sample(g_sampler, uv).xyz;
		return 2.0 * shift.z ;
	}

	// Only consider the direction light, which will be at SceneLight[0]
	float3 viewDir = normalize(CameraPos - input.worldpos.xyz);
	float3 lightDir = normalize(SceneLights[0].direction.xyz);
	float3 halfVec = normalize(viewDir + lightDir);

	float3 strength = SceneLights[0].strength.xyz;
	

	float cosL = max(0.0, dot(lightDir, normal));
	float cosV = max(0.0, dot(viewDir, normal));
	float cosLV = max(0.0, dot(viewDir, lightDir));
	float cosD = max(0.0, dot(halfVec, lightDir));
	
	// Subsurface scattering
	float subsurfaceBase = 1.0;
	float subsurfaceSun  = 3.0;
	float towardSun = pow(cosV, 5.0);
	float ssIntensity = 20.5 * max(0.00, input.worldpos.z); // TODO normalize this
	float subsurface = (subsurfaceBase + subsurfaceSun * towardSun) * ssIntensity;

	const float3 water0 = float3(0.1, 0.1, 0.8);
	const float3 water1 = float3(0.4, 0.6, 0.98);
	
	// return float4(cosL * water0 * 10.0 + 1.0 * cosV * water1, 1.0);
	float3 deepCol = water0 *(0.02 + cosD) * strength;
	float3 scatterCol =  subsurface * water1 * strength;
	
	const float ior = 0.08;
	float F = ior + (1.0 - ior) * pow(1.f - cosD, 5);
	float3 reflection = F * cosL * strength / max(0.2, cosV) / 4.0;
	return float4(scatterCol + reflection + deepCol, 1.0);
}