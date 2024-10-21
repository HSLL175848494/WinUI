#ifndef HS_DENUMSTRUCT
#define HS_DENUMSTRUCT

//inc
#include <DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>

namespace HSLL
{

#ifndef HS_COMPTR

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

#define HS_COMPTR
#endif // !HS_COMPTR

#define HSLL_MATH_PI  3.141592653589793

	//����ɫֵתΪ��������Ϊ0-1����ԭ�������ϳ���255.0��
	struct FloatColor
	{
		float R;
		float G;
		float B;
		float A;
	};

	enum TextureStyle	//������ʽ
	{
		TextureStyle_Pos,//����
		TextureStyle_Tile,//ƽ��
		TextureStyle_Center,//����
		TextureStyle_Stretch,//�������
		TextureStyle_Stretch_Align//��������
	};

	struct FactorTexture//��Ч����
	{
		float tFactor;//͸������
		float eFactor;//�ع�����
		float unkonwn1;//δ֪
		float unkonwn2;//δ֪
	};

	struct FloatRect//�����;�������
	{
		float Left;
		float Top;
		float Right;
		float Bottom;
	};

	struct FloatPoint//�����͵�
	{
		float X;
		float Y;
	};

	struct RotatePoint
	{
		float X;
		float Y;

		static const RotatePoint TextureCenter;
		static const RotatePoint LeftCenter;
		static const RotatePoint RightCenter;
		static const RotatePoint TopCenter;
		static const RotatePoint BottomCenter;
		static const RotatePoint LeftTop;
		static const RotatePoint RightTop;
		static const RotatePoint LeftBottom;
		static const RotatePoint RightBottom;

		bool operator ==(const RotatePoint& Other);
	};

	struct  VertexTexture//������
	{
		float X;//��������[-1,1]
		float Y;
		float U;//����uv����[0,1]
		float V;
	};

	struct  VertexNormal//һ�㶥��
	{
		float X;//��������[-1,1]
		float Y;
		unsigned int Colors;//��ɫֵ
		float Unknown1;//���
	};


	class TaskBase
	{
	protected:

		friend class D3d11Render;
		friend class WinBase32;

		bool NeedUpdate;//��Ҫ����
		const unsigned int VerticesNum;//��������
		const unsigned int VertexPersize;//���������С
		const unsigned int ParamIndex;//���Ʋ�������
		ComPtr<ID3D11Buffer> pVertexBuffer;//���㻺��(�����ϵ)

		TaskBase(unsigned int VerticesNum, unsigned int VertexPersize, unsigned int ParamIndex);

		virtual void FuncAdditon(D3d11Render* pRender)=0;
		virtual void FillVerticeBuffer(DWORD Width, DWORD Height,void* FillBuffer) = 0;//���void* pBuffer���㻺��
	};



	//ͼԪ���˲���
	struct DrawParam
	{
		ComPtr<ID3D11PixelShader> pPixelShader;//������ɫ��
		ComPtr<ID3D11VertexShader> pVertexShader;//������ɫ��
		ComPtr<ID3D11InputLayout> pInputLayout;//�������
		D3D11_PRIMITIVE_TOPOLOGY Topology;//��������
	};

	class TaskTexture :public TaskBase
	{
	private:

		friend class D3d11Render;

		ComPtr<ID3D11Texture2D> pTexture2D;//����
		ComPtr<ID3D11ShaderResourceView> pSourse;//Ŀ����Ⱦ��ͼ
		TextureStyle Style;//������ʽ

	public:

		const float tFactor;//͸������0-1
		const float eFactor;//�ع�����0-2��1���������ع⣬0ȫ����1ȫ��
		const float Scalling;//������
		const unsigned int Width;//������
		const unsigned int Height;//����߶�
		const float Angle;//�Ƕ�
		const FloatRect  tInvalidRect;//������Ч����
		const FloatPoint Pos;//����
		const RotatePoint rPoint;//��ת����

		//set
		void  Set_tFactor(float tFactor);
		void  Set_eFactor(float eFactor);
		void  Set_Scalling(float Scalling);
		void  Set_Angle(float Angle);
		void  Set_Pos(FloatPoint Pos);
		void  Set_RotatePoint(RotatePoint rPoint);
		void  Set_tInvalidRect(FloatRect tInvalidRect);

		TaskTexture(unsigned int Width, unsigned int Height, TextureStyle tStyle, float tFactor, float eFactor, float Scalling,
			float Angle, FloatPoint Pos, RotatePoint rPoint, FloatRect tInvalidRect);

		void PointRotating(float* X, float* Y, float RotateX, float RotateY, float AngleRadian);//��ת��

		void FillVerticeBuffer(DWORD Width, DWORD Heigth,void* FillBuffer) override;

		void FuncAdditon(D3d11Render* pRender)override;
	};
}

#endif //HS_DENUMSTRUCT
