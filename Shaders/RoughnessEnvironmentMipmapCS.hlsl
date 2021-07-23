#define PI 3.141592653589793f
#define TwoPI 6.283185307179586f

static const uint NumSamples = 1024;
static const float InvNumSamples = 1.0f / NumSamples;


cbuffer InfoBuffer : register(b0) 
{
    uint NumMips;	// How many mipmap we are going to use
    float InvResolution;
};

// EnvMap is also Mip 0 for roughness 0
TextureCube<float4> EnvMap : register(t0);

RWTexture2DArray<float4> Mip1 : register(u0);
RWTexture2DArray<float4> Mip2 : register(u1);
RWTexture2DArray<float4> Mip3 : register(u2);
RWTexture2DArray<float4> Mip4 : register(u3);
RWTexture2DArray<float4> Mip5 : register(u4);
RWTexture2DArray<float4> Mip6 : register(u5);
RWTexture2DArray<float4> Mip7 : register(u6);



SamplerState BilinearSampler : register(s0);

// Sequence of points in [0, 1]x[0, 1]
float2 SampleHammersley(uint i) {
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

float3 local2world(float3 H, float3 N) {
    float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0); // Pick a random vector
    float3 U = normalize(cross(UpVector, N));   
    float3 V = cross(N, U);
    // (U,V,N) : (0, 0, 1) -> N is the transform from local to world
    return U * H.x + V * H.y + N * H.z;
}

float3 SampleGGX(float2 seed, float Roughness, float3 N) {
    return local2world(SampleRoughnessSphere(seed, Roughness), N);
}



float3 PreFilterEnvMap(float Roughness, float3 N) {
    float3 V = N;
    
    float3 light = float3(0.0, 0.0, 0.0);
    float weight = 0;

    for(uint i = 0; i < NumSamples; ++i) {
        float2 seedi = SampleHammersley(i);
        float3 H = SampleGGX(seedi, Roughness, N);
        float3 L = 2 * dot(V, H) * H - V;

        float NoL = saturate(dot(N, L));
        if(NoL > 0) {
            light += EnvMap.SampleLevel(BilinearSampler, L, 0).rgb * NoL;
            weight += NoL;
        }
    }
    return light / weight;
}

# define MapMipFactory(level)\
void PrefiterMapMip##level(float2 cube_coor, float2 uv, float Roughness) { \
    float3 pixel_pos; \
    float3 Color = float3(1.0, 0.0, 0.0); \
    pixel_pos = normalize(float3(1.0, uv.y, -uv.x)); \
    Color = PreFilterEnvMap(Roughness, pixel_pos); \
    Mip##level[uint3 (cube_coor, 0)] = float4(Color, 1.0); \
    \
    pixel_pos = normalize(float3(-1.0, uv.y, uv.x)); \
    Color = PreFilterEnvMap(Roughness, pixel_pos); \
    Mip##level[uint3 (cube_coor, 1)] = float4(Color, 1.0); \
    \
    pixel_pos = normalize(float3(uv.x, 1.0, -uv.y));\
    Color = PreFilterEnvMap(Roughness, pixel_pos); \
    Mip##level[uint3 (cube_coor, 2)] = float4(Color, 1.0);\
    \
    pixel_pos = normalize(float3(uv.x, -1.0, uv.y));\
    Color = PreFilterEnvMap(Roughness, pixel_pos); \
    Mip##level[uint3 (cube_coor, 3)] = float4(Color, 1.0);\
    \
    pixel_pos = normalize(float3(uv, 1.0));\
    Color = PreFilterEnvMap(Roughness, pixel_pos); \
    Mip##level[uint3 (cube_coor, 4)] = float4(Color, 1.0);\
    \
    pixel_pos = normalize(float3(-uv.x, uv.y, -1.0));\
    Color = PreFilterEnvMap(Roughness, pixel_pos); \
    Mip##level[uint3 (cube_coor, 5)] = float4(Color, 1.0);\
}\

// Color = PreFilterEnvMap(Roughness, pixel_pos); 
MapMipFactory(1);
MapMipFactory(2);
MapMipFactory(3);
MapMipFactory(4);


[numthreads( 8, 8, 1 )]
void CSMain(uint3 DispatchThreadID : SV_DispatchThreadID,  uint GI : SV_GroupIndex)
{
    uint2 cube_coor = DispatchThreadID.xy;
    float2 uv =  2.0 * (2.0 * DispatchThreadID.xy * InvResolution - 0.5);
    uv.y = -uv.y;

    PrefiterMapMip1(cube_coor, uv, 0.1);
}