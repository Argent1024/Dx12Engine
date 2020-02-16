cbuffer Camera : register(b0) 
{
	float4x4 ViewMatrix;
};


struct VSParticleIn
{
    float4 color    : COLOR;
    uint id            : SV_VERTEXID;
};

struct VSParticleDrawOut
{
    float3 pos            : POSITION;
    float4 color        : COLOR;
};

struct GSParticleDrawOut
{
    float4 color        : COLOR;
    float4 pos            : SV_POSITION;
};

struct PSParticleDrawIn
{
    float4 color        : COLOR;
};

struct PosVelo
{
    float4 pos;
};

StructuredBuffer<PosVelo> g_bufPosVelo : register(t0);

cbuffer cb1
{
    static float g_fParticleRad = 0.015f;
};

cbuffer cbImmutable
{
    static float3 g_positions[4] =
    {
        float3(-1, 1, 0),
        float3(1, 1, 0),
        float3(-1, -1, 0),
        float3(1, -1, 0),
    };
    
    static float2 g_texcoords[4] =
    { 
        float2(0, 0), 
        float2(1, 0),
        float2(0, 1),
        float2(1, 1),
    };
};

//
// Vertex shader for drawing the point-sprite particles.
//
VSParticleDrawOut VSParticleDraw(VSParticleIn input)
{
    VSParticleDrawOut output;
    
    output.pos = g_bufPosVelo[input.id].pos.xyz;
    output.color = input.color;
    
    return output;
}

//
// GS for rendering point sprite particles.  Takes a point and turns 
// it into 2 triangles.
//
[maxvertexcount(4)]
void GSParticleDraw(point VSParticleDrawOut input[1], inout TriangleStream<GSParticleDrawOut> SpriteStream)
{
    GSParticleDrawOut output;
    
    // Emit two new triangles.
	for (int i = 0; i < 4; i++)
	{
		float3 position = g_positions[i] * g_fParticleRad;
		position = position + input[0].pos;
		output.pos = mul(float4(position, 1.0), ViewMatrix);

		output.color = float4(input[0].pos, 1.0);//input[0].color;
		SpriteStream.Append(output);
	}
    SpriteStream.RestartStrip();
}

//
// PS for drawing particles. Use the texture coordinates to generate a 
// radial gradient representing the particle.
//
float4 PSParticleDraw(PSParticleDrawIn input) : SV_Target
{
    return 1.5 * input.color;
}