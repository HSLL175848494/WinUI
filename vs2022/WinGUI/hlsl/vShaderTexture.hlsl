struct VSOut
{
    float2 uv : TEXCOORD;
    float4 pos : SV_POSITION;
};

VSOut main(float2 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOut vsout;
    vsout.uv = uv;
    vsout.pos = float4(pos, 1.0f, 1.0f);
    return vsout;
}