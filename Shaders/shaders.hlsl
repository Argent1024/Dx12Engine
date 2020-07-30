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
	float3 BaseColor;
	
	float Roughness;
	float Metallic;
	float Specular;

	// TODO add texture stuff later
}

// TODO Material Textures

/********************** Object & Material Descriptor Table End ****************************/


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
	float4 camerapos: POSITION1;
	float4 screenpos: POSITION2;
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
	result.camerapos = pos;

	pos = mul(pos, projection);

	result.position = pos;
	result.screenpos = pos;
	
	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}


// Calculate brdf
float3 CalculateBRDF(float3 viewDir, float3, lightDir, float3 normal) {

	// float3 halfVec = normalize(viewDir + lightDir);
	return BaseColor;
	/*float cos_L = dot(lightDir, normal);  // Theta L, light and normal
	float cos_V = dot( viewDir, normal);  // Theta V,  view and normal
	float cos_H = dot( halfVec, normal);  // Theta H,  half and normal
	float cos_D = dot(lightDir, halfVec); // Theta D,  half and light

	if (cos_T <= 0.f) {
		return 0.f;
	}
	
	float3 baseColor = BaseColor;
	float roughness = Roughness;

	float FD = 0.5f + 2.f* cos_D * cos_D * roughness;
	// Diffuse Term
	float3 diffuse = baseColor * (1 + (FD - 1) * pow(1 - cos_L, 5)) * (1 + (FD - 1) * pow(1 - cos_V, 5))
	
	// Specular Term
	return diffuse;*/
}



// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	// return float4(1.0, 0.0, 0.0, 1.0);
	if (debugnormal) {
		return (float4(input.normal, 1.0f) + 1.0f) / 2.0f;
	}

	if (debugpos) {
		return (input.worldpos + 1.0f) / 2.0f;
	}

	float3 viewDir = normalize(input.worldpos - CameraPos);

	float3 lightDir = SceneLights[0].direction.xyz;
	float4 strength = SceneLights[0].strength;
	float3 normal = input.normal;
	
	float cos_T = max(dot(lightDir, normal), 0);

	float3 brdf = CalculateBRDF(viewDir, lightDir, normal);
	
	return float4(BaseColor, 1) * cos_T;

	/*float cos_ln = max(dot(lightDir, normal), 0);
	return float4(cos_ln * strength * BaseColor, 1.0f);*/
}