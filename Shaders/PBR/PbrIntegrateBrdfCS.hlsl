#include "PBRcommon.hlsl"

float GeometrciAttenuation(float Roughness, float NoV, float NoL)
{
    // For IBL k = Roughness^2 / 2
	float k = Roughness * Roughness / 2;
	float Gv = NoV / (NoV * (1 - k) + k);
	float Gl = NoL / (NoL * (1 - k) + k);
	return Gv * Gl;
}


float2 IntegrateBRDF(float Roughness, float NoV)
{
	// Calculate the integral of BRDF in local coordinate system, N = (0, 0, 1)
	float3 V;
	V.z = NoV;
	V.y = 0;
	V.x = sqrt(1.0 - NoV * NoV);
	
	float A = 0;
	float B = 0;
	
	for (uint i = 0; i < NumSamples; ++i)
    {
        float2 seed = SampleHammersley(i);
        float3 H = SampleRoughnessSphere(seed, Roughness);
		
        float3 L = 2.0 * dot(V, H) * H - V;
		
        float NoL = saturate(L.z);
        float NoH = saturate(H.z);
        float VoH = saturate(dot(V, H));

		// TODO why NoL will less than 0?
        if (NoL > 0)
        {
            float G = GeometrciAttenuation(Roughness, NoV, NoL);
            G = G * VoH / (NoH * NoV);
			
            float Fc = pow(1.0 - VoH, 5);
            A += (1.0 - Fc) * G;
            B += Fc * G;
        } 
	}
	return float2(A, B) / NumSamples;
}



cbuffer InfoBuffer : register(b0)
{
	float InvU;
	float InvV;
};
RWTexture2D<float2> BrdfTexture : register(u0);

[numthreads(8, 8, 1)]
void CSMain(uint3 DispatchThreadID : SV_DispatchThreadID, uint GI : SV_GroupIndex)
{
	uint2 texCoor = DispatchThreadID.xy;
    float Roughness = (texCoor.x + 0.5) * InvU;
    float NoV = (texCoor.y + 0.5) * InvV;
	BrdfTexture[texCoor] = IntegrateBRDF(Roughness, NoV);

}