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
Texture2D<float4> EnvMap : register(t2);
Texture2D<float4> FoamMap : register(t3);

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
	float foam		: TEXCOORD1;
	
};

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	float scale = 1.0;

	// float3 shift = DisplacementTex.SampleLevel(g_sampler, input.fuv, 0).xyz / 25.0;
	float3 shift = DisplacementTex[input.uv].xyz / scale;
	
	// Simple way to calculate the Jacobian at this point
	const float unitSize = 0.1;
	const float FoamSize = 0.3;

	float2 x0 = DisplacementTex[input.uv + uint2(1, 0)].xy;
	float2 x1 = DisplacementTex[input.uv - uint2(1, 0)].xy;
	float2 y0 = DisplacementTex[input.uv + uint2(0, 1)].xy;
	float2 y1 = DisplacementTex[input.uv - uint2(0, 1)].xy;

	float2 dDx = (x0 - x1) / unitSize;
	float2 dDy = (y0 - y1) / unitSize;

	float J = ((1.0 + dDx.x) * (1.0 + dDy.y) - dDx.y * dDy.x) / 5.0;
	if( J < FoamSize) {
		result.foam = min(1.0, 2.0 * (FoamSize - J));
	} else {
		result.foam = 0.0;
	}


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


float2 dir_env_coor(float3 dir) {
	float v = asin(dir.z);
	float u = atan2(dir.y / cos(v), dir.x / cos(v));
	return float2(u / (2 * 3.1415926) + 0.5,v / 3.1415926);
}


// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{	
	const float PI = 3.14159265;
	float2 uv = float2(input.uv);
	float3 normal = normalize(NormalTex.Sample(g_sampler, uv).xyz);
	float3 shift = DisplacementTex.Sample(g_sampler, uv).xyz;

	if (debugnormal) {
		float3 test = EnvMap.Sample(g_sampler, uv).xyz;
		return input.foam;
		return float4(test, 1.0) ;
	}

	if(debugpos) {
		return 10.0 * shift.z;
		return 10.0 * float4(shift, 1.0);
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

	// float cosHV = max(0.0, dot(halfLN, viewDir));
	
	const float3 water0 = float3(0.1, 0.3, 0.6);
	const float3 water1 = float3(0.0, 0.95, 1.0);
	const float3 FoamColor = float3(1.0, 1.0, 1.0);

	// Subsurface scattering
	float subsurfaceBase = 0.1;
	float subsurfaceSun  = 1.0;

	float towardSun = min(pow(cosV, 2.0), 0.7);	// More sss when view directly to the wave
	float sssIntensity = min(1.0, 50.0 * max(0.0, shift.z));//20.0 *  pow(input.foam, 2.0);
	float subsurface = (subsurfaceBase  + subsurfaceSun * towardSun) * sssIntensity;
	float3 WaveColor = lerp(water1, FoamColor, input.foam);

	float3 scatterCol =  subsurface * WaveColor * strength;
	
	// return float4(cosL * water0 * 10.0 + 1.0 * cosV * water1, 1.0);
	float3 deepCol = water0 * cosL * strength;


	float Specular = 93.0;
	float specularColor = min(0.5, pow(max(0.0, dot( reflect(-lightDir, normal), -viewDir)), Specular));
	specularColor = min(specularColor, 0.5);

	float3 reflectionDir = reflect(-viewDir, normal); //normal);
	float2 envUV = dir_env_coor(reflectionDir);

	float f = 1/1.3;
	float F = f + (1 - f) * pow(1.0 - cosV, 5);
	float3 envCol = 0.2 * F * EnvMap.Sample(g_sampler, envUV).xyz;


	float3 mixWaveTop = scatterCol + pow(input.foam+0.1, 2.0);

	// return float4(foamCol, 1.0);
	return float4(envCol + deepCol + mixWaveTop + specularColor, 1.0);
}