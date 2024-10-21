Texture2D<float4> tex : t0; // 第0个纹理单元
SamplerState splr; // 采样器

cbuffer PConstantBuffer : register(b0) // 有效矩形常量缓冲
{
    float tFactor; //透明因子
    float eFactor; //曝光因子
    float Unknown1; //补齐16字节的整数倍
    float Unknown2; //补齐16字节的整数倍
};


float4 main(float2 uv : TEXCOORD, float4 pos : SV_POSITION) : SV_TARGET
{
    // 采样纹理颜色
    float4 color = tex.Sample(splr, uv);
    
    // 曝光因子处理
    if (eFactor > 1.0)
    {
        color.r += (1 - color.r) * (eFactor - 1);
        color.g += (1 - color.g) * (eFactor - 1);
        color.b += (1 - color.b) * (eFactor - 1);
    }
    else
    {
        color.r *= eFactor;
        color.g *= eFactor;
        color.b *= eFactor;
    }

    color.a*=tFactor; //透明的处理
    
    return color; // 返回颜色值
}