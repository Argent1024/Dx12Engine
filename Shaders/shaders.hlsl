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
	float3 BaseColor;

	float Roughness;
	float Metallic;
	float Specular;
	// TODO add texture stuff later
}

Texture2D Material_BaseColor : register(t0);
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


// Intersection point's data, helper struct for BRDF
struct IntersectionPoint
{
	float3 BaseColor;

	float Roughness;
	float Metallic;
	float Specular;

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
float3 CalculateBRDF(float3 viewDir, 
					float3 lightDir, 
					float3 normal, 
					IntersectionPoint isect)
{

	float3 halfVec = normalize(viewDir + lightDir);
	
	float cos_L = dot(lightDir, normal);  // Theta L, light and normal
	float cos_V = dot( viewDir, normal);  // Theta V,  view and normal
	float cos_H = dot( halfVec, normal);  // Theta H,  half and normal
	float cos_D = dot(lightDir, halfVec); // Theta D,  half and light
	
	if (cos_L <= EPLISION || cos_V <= EPLISION || cos_D <= EPLISION || cos_H <= EPLISION) {
		return 0.f;
	}
	
	float3 baseColor = isect.BaseColor;
	
	float roughness = isect.Roughness;
	float metallic = isect.Metallic;
	float specular = isect.Specular;
	

	float FD = 0.5f + 2.f * cos_D * cos_D * roughness;
	// Diffuse Term
	float3 diffuse = baseColor * (1 + (FD - 1) * pow(1 - cos_L, 5)) * (1 + (FD - 1) * pow(1 - cos_V, 5));
	
	// Specular Term
	// microfacet distribution
	float alpha2 = pow(roughness, 4);
	float cosH2 = pow(cos_H, 2);
	float sinH2 = 1.f - cosH2;
	float D = alpha2 / pow(alpha2 * cosH2 + sinH2, 2);

	// Fresnel reflection
	float F0 = 0.08 * specular; 
	float F = 0;
	// Ignore specular if it's small
	if (specular > EPLISION) {
		F = F0 + (1.f - F0) * pow(1.f - cos_D, 5);
	}

	float G = 1.0f;
	 
	return (diffuse * cos_L + D * F * G / (4.f * cos_V)) / 3.1415f;
}



// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	// return Material_BaseColor.Sample(g_sampler, input.uv);
	
	if (debugnormal) {
		return (float4(input.normal, 1.0f) + 1.0f) / 2.0f;
	}

	if (debugpos) {
		return (input.worldpos + 1.0f) / 2.0f;
	}

	// Render default

	float3 viewDir = normalize(CameraPos - input.worldpos.xyz);

	float3 lightDir = SceneLights[0].direction.xyz;
	float4 strength = SceneLights[0].strength;
	
	// Get and apply Transform to normal
	float3 normal = mul(float4(input.normal, 0.f), modelTransformation).xyz;
	


	// Calculate surface material's data
	IntersectionPoint isect;
	isect.BaseColor = BaseColor;
	//isect.BaseColor = Material_BaseColor.Sample(g_sampler, input.uv);
	isect.Roughness = Roughness;
	isect.Specular = Specular;
	
	float3 brdf = CalculateBRDF(viewDir, lightDir, normal, isect);
	return 2 * float4(brdf + 0.2 * isect.BaseColor, 1.0f);
	
}