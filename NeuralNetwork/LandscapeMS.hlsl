cbuffer PlaneCB : register(b0)
{
    float2 planeSize; // (width, depth) in world units
    uint gridRes; // #cells across one edge BEFORE tess (e.g. 64)
    float dispScale; // world-space height scale
    float4x4 viewProj; // usual MVP for clip output
};

Texture2D heightMap : register(t0);
SamplerState heightSampler : register(s0);

struct VSOutput
{
    float4 pixelPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : UV;
    float4 tangent : TANGENT;
};

//#define BASE_RES    64              // max cells per tile edge
//#define SUBDIV      2               // 1 → 4 quads; 2x2 = 4 verts per cell
//#define THREADS_X   (BASE_RES+1)    // one thread per ORIGINAL vertex
//#define THREADS_Y   1
//#define THREADS_Z   1
//static const uint MaxVerts = (BASE_RES + 1) * (BASE_RES + 1) * SUBDIV * SUBDIV;
//static const uint MaxTris = (BASE_RES) * (BASE_RES) * SUBDIV * SUBDIV * 2;
static const uint MaxCells = 128;
static const uint MaxVerts = MaxCells;
static const uint MaxTris = MaxCells / 2;

// groupshared buffers for smoothing
//groupshared float3 gPos[(BASE_RES + 1) * (BASE_RES + 1)];
//groupshared float3 gNrm[(BASE_RES + 1) * (BASE_RES + 1)];
groupshared float3 gPos[64];
groupshared float3 gNrm[64];

struct ThreadIDs
{
    uint3 tid : SV_GroupThreadID;
    uint3 dtid : SV_DispatchThreadID;
    uint3 gid : SV_GroupID;
};

#define THREADS_X 8
#define THREADS_Y 8
#define THREADS_Z 1

[outputtopology("triangle")]
[numthreads(THREADS_X, THREADS_Y, THREADS_Z)]
void main(ThreadIDs threadIds,
           out vertices VSOutput verts[MaxVerts],
           out indices uint3 tris[MaxTris])
{
    SetMeshOutputCounts(64, 128); // must be within hardware limits

    //------------------------------------------------------------
    // 1.  Generate base grid vertex (before subdivision)
    //------------------------------------------------------------
    float2 cell = (float2) threadIds.tid.xy; // 0..BASE_RES
    float2 uv0 = cell / gridRes; // 0..1 in tile
    float2 posXZ = (uv0 - 0.5) * planeSize; // center plane at origin
    float3 basePos = float3(posXZ.x, 0.0, posXZ.y);
    float3 baseNrm = float3(0, 1, 0);

    uint vIndex = threadIds.tid.x; // row = 0 (1-D threads)
    gPos[vIndex] = basePos;
    gNrm[vIndex] = baseNrm;
    
    //------------------------------------------------------------
    // 2.  Displace along normal using height map
    //------------------------------------------------------------
    float height = heightMap.Sample(heightSampler, uv0).r;
    basePos.y += height * dispScale;
    gPos[vIndex] = basePos; // update shared for smoothing

    //------------------------------------------------------------
    // 3.  One-iteration Laplacian smooth
    //------------------------------------------------------------
    GroupMemoryBarrierWithGroupSync();
    if (threadIds.tid.x > 0 && threadIds.tid.x < gridRes)          // interior only
    {
        float3 left = gPos[vIndex - 1];
        float3 right = gPos[vIndex + 1];
        float3 avg = (left + right + basePos) / 3.0;
        basePos = lerp(basePos, avg, 0.5); // λ = 0.5
    }
    gPos[vIndex] = basePos;
    GroupMemoryBarrierWithGroupSync();

    //------------------------------------------------------------
    // 4.  Emit subdivided vertices (SUBDIV=2 → 4 verts per cell)
    //------------------------------------------------------------
    // Map original vertex id to final index in output buffer
    uint finalVID = threadIds.tid.x; // remap if > MaxVerts

    VSOutput v;
    v.uv = uv0;
    v.normal = float3(0, 1, 0); // recompute if needed
    v.pixelPos = mul(viewProj, float4(basePos, 1));
    verts[finalVID] = v;

    //------------------------------------------------------------
    // 5.  Emit triangles (one thread per cell corner == tri writer)
    //------------------------------------------------------------
    if (threadIds.tid.x < gridRes)      // cell writer
    {
        uint base = threadIds.tid.x; // row=0
        uint3 t0 = uint3(base, base + 1, base + gridRes + 1);
        uint3 t1 = uint3(base + 1, base + gridRes + 2, base + gridRes + 1);
        uint triID = base * 2;
        tris[triID] = t0;
        tris[triID + 1] = t1;
    }
}

uint GetVertexIndex(uint2 coord, uint xres)
{
    return xres * coord.y + coord.x;
}

VSOutput MeshPrimitivePlane(ThreadIDs threadIds, float size, uint2 resolution)
{    
    uint2 gvid = threadIds.dtid.xy;
    uint2 lvid = threadIds.tid.xy;

    uint2 p0 = uint2(lvid.x, lvid.y);
    uint2 p1 = uint2(lvid.x + 1, lvid.y);
    uint2 p2 = uint2(lvid.x + 1, lvid.y + 1);
    uint2 p3 = uint2(lvid.x, lvid.y + 1);
    uint i0 = p0.x + p0.y * THREADS_X;
    uint i1 = p1.x + p1.y * THREADS_X;
    uint i2 = p2.x + p2.y * THREADS_X;
    uint i3 = p3.x + p3.y * THREADS_X;
    
    uint3 t0 = uint3(i0, i1, i3); // Tri 1: p0, p1, p3
    uint3 t1 = uint3(i0, i2, i3); // Tri 2: p0, p2, p3
    
    
    
    VSOutput output;

    float2 uv = (float2) gvid / float2(resolution - 1);
    float2 gridPos = (uv - 0.5) * size;
    float3 worldPos = float3(gridPos, 0);

    output.pixelPos = mul(viewProj, float4(worldPos, 1.0));
    output.worldPos = worldPos;
    output.normal = float3(0, 0, 1);
    output.tangent = float4(1, 0, 0, 1);
    output.uv = uv;
    return output;
}