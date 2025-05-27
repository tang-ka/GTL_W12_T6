
struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_Input main(uint VertexID : SV_VertexID)
{
    PS_Input Output;

    float2 QuadPositions[4] = {
        float2(-1, -1),
        float2(1, -1),
        float2(-1,  1),
        float2(1,  1)
    };

    float2 UVs[4] = {
        float2(0, 1),
        float2(1, 1),
        float2(0, 0),
        float2(1, 0)
    };

    uint Indices[6] = {
        0, 2, 1,
        1, 2, 3
    };

    uint Index = Indices[VertexID];
    Output.Position = float4(QuadPositions[Index], 0, 1);
    Output.UV = UVs[Index];

    return Output;
}
