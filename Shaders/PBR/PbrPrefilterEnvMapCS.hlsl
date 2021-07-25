#include "PbrCommon.hlsl"

cbuffer InfoBuffer : register(b0) 
{
    uint NumRoughness;	// How many mipmap we are going to use
    float InvResolution;
};

// EnvMap is also Mip 0 for roughness 0
TextureCube<float4> EnvMap : register(t0);

RWTexture2DArray<float4> Mip1 : register(u0);
RWTexture2DArray<float4> Mip2 : register(u1);
RWTexture2DArray<float4> Mip3 : register(u2);
RWTexture2DArray<float4> Mip4 : register(u3);



SamplerState BilinearSampler : register(s0);


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
    float2 uv = 2.0 * (2.0 * DispatchThreadID.xy * InvResolution - 0.5);
    uv.y = -uv.y;

    PrefiterMapMip1(cube_coor, uv, 0.125);
    if (NumRoughness == 1) { return; }
    
    
    // checks x & y are even
    if ((GI & 0x9) == 0)
    {
        cube_coor = DispatchThreadID.xy / 2;
        PrefiterMapMip2(cube_coor, uv, 0.25);
    }
    if (NumRoughness == 2) { return; }
    
    
    // This bit mask (binary: 011011) checks that X and Y are multiples of four.
    if ((GI & 0x1B) == 0)
    {
        cube_coor = DispatchThreadID.xy / 4;
        PrefiterMapMip3(cube_coor, uv, 0.5);
    }
    if (NumRoughness == 3) { return; }
    
    
     // This bit mask (binary: 011011) checks that X and Y are multiples of 8.
    if ((GI & 0x1B) == 0)
    {
        cube_coor = DispatchThreadID.xy / 8;
        PrefiterMapMip4(cube_coor, uv, 1.0);
    }
}