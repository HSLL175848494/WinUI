Texture2D<float4> tex : t0; // ��0������Ԫ
SamplerState splr; // ������

cbuffer PConstantBuffer : register(b0) // ��Ч���γ�������
{
    float tFactor; //͸������
    float eFactor; //�ع�����
    float Unknown1; //����16�ֽڵ�������
    float Unknown2; //����16�ֽڵ�������
};


float4 main(float2 uv : TEXCOORD, float4 pos : SV_POSITION) : SV_TARGET
{
    // ����������ɫ
    float4 color = tex.Sample(splr, uv);
    
    // �ع����Ӵ���
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

    color.a*=tFactor; //͸���Ĵ���
    
    return color; // ������ɫֵ
}