#define PI 3.141592653589793f
#define TwoPI 6.283185307179586f

static const uint NumSamples = 1024;
static const float InvNumSamples = 1.0f / NumSamples;

// Sequence of points in [0, 1]x[0, 1]
float2 SampleHammersley(uint i)
{
	float j = reversebits(i) * 2.3283064365386963e-10;
	return float2(i * InvNumSamples, j);
}

// Use a random seed to generate a direction in the Hemisphere
float3 SampleRoughnessSphere(float2 seed, float Roughness)
{
	float a = Roughness * Roughness;

	float Phi = TwoPI * seed.x;
	float cosT = sqrt((1 - seed.y) / (1 + (a * a - 1) * seed.y));
	float sinT = sqrt(1 - cosT * cosT);
	return float3(sinT * cos(Phi), sinT * sin(Phi), cosT);
}

float3 local2world(float3 H, float3 N)
{
	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0); // Pick a random vector
	float3 U = normalize(cross(UpVector, N));
	float3 V = cross(N, U);
    // (U,V,N) : (0, 0, 1) -> N is the transform from local to world
	return U * H.x + V * H.y + N * H.z;
}

float3 SampleGGX(float2 seed, float Roughness, float3 N)
{
	return local2world(SampleRoughnessSphere(seed, Roughness), N);
}

