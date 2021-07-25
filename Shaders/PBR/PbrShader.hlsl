cbuffer CameraData : register(b0)
{
	float4x4 projection;
	float4x4 view;
	float3 CameraPos;
}

cbuffer ObjectInfo : register(b1)
{
	float4x4 modelTransformation;
}
TextureCube<float4> EnvMap : register(t0);


cbuffer MaterialInfo : register(b0, space1)
{
	float3 BaseColor;

	float Roughness;
	float Metallic;
	float Specular;

	bool UseBaseTexture;
	bool UseNormalTexture;
}

cbuffer RenderSetting : register(b0, space2)
{
	bool DebugNormal;
}

Texture2D<float4> BaseColorTexture : register(t0, space1);
Texture2D<float2> MaterialBRDF : register(t3, space1);

SamplerState g_sampler : register(s0);


struct VSInput
{
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};


struct PSInput
{
	float4 position : SV_POSITION;
	float4 worldpos : POSITION0;
	float4 camerapos : POSITION1;
	float4 screenpos : POSITION2;
	float3 normal : COLOR0;
	float2 uv : TEXCOORD0;
};


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
	
    float4 n = float4(input.normal, 0.0f);
    n = mul(n, modelTransformation);
    result.normal = normalize(n);
    
	result.uv = input.uv;
	return result;
}


float3 PrefilterEnvMap(float Roughness, float3 N)
{
    const float MAX_REFLECTION_LOD = 4.0;
    float level = Roughness * MAX_REFLECTION_LOD;
    level = min(level, MAX_REFLECTION_LOD);
    return EnvMap.SampleLevel(g_sampler, N, level);
}

float4 PSMain(PSInput input) : SV_TARGET
{
	// debug normal
	// return (float4(input.normal, 1.0f) + 1.0f) / 2.0f;
	
	// UV
	// return float4(input.uv, 0, 1.0);
	
	float3 N = input.normal;
	float3 V = normalize(CameraPos - input.worldpos.xyz);
	
	float F0 = Specular;
	float NoV = saturate(dot(N, V));
	
	float2 BRDF = MaterialBRDF.Sample(g_sampler, float2(Roughness, NoV));
	
	float3 diffuse = BaseColor;
	if (UseBaseTexture) {
		float3 diffuse = BaseColorTexture.Sample(g_sampler, input.uv);
	}
	
	float3 prefilteredColor = PrefilterEnvMap(Roughness, N);

    // return float4(BRDF.x, BRDF.y, 0.0, 1.0);
	return float4(prefilteredColor * (F0 * BRDF.x + BRDF.y), 1.0);
}