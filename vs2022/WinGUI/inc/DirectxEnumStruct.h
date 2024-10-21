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

	//将颜色值转为浮点数即为0-1（在原本基础上除以255.0）
	struct FloatColor
	{
		float R;
		float G;
		float B;
		float A;
	};

	enum TextureStyle	//纹理样式
	{
		TextureStyle_Pos,//坐标
		TextureStyle_Tile,//平铺
		TextureStyle_Center,//中心
		TextureStyle_Stretch,//填充拉伸
		TextureStyle_Stretch_Align//对齐拉伸
	};

	struct FactorTexture//有效区域
	{
		float tFactor;//透明因子
		float eFactor;//曝光因子
		float unkonwn1;//未知
		float unkonwn2;//未知
	};

	struct FloatRect//浮点型矩形区域
	{
		float Left;
		float Top;
		float Right;
		float Bottom;
	};

	struct FloatPoint//浮点型点
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

	struct  VertexTexture//纹理顶点
	{
		float X;//顶点坐标[-1,1]
		float Y;
		float U;//纹理uv坐标[0,1]
		float V;
	};

	struct  VertexNormal//一般顶点
	{
		float X;//顶点坐标[-1,1]
		float Y;
		unsigned int Colors;//颜色值
		float Unknown1;//填充
	};


	class TaskBase
	{
	protected:

		friend class D3d11Render;
		friend class WinBase32;

		bool NeedUpdate;//需要更新
		const unsigned int VerticesNum;//顶点数量
		const unsigned int VertexPersize;//单个顶点大小
		const unsigned int ParamIndex;//绘制参数索引
		ComPtr<ID3D11Buffer> pVertexBuffer;//顶点缓冲(不需关系)

		TaskBase(unsigned int VerticesNum, unsigned int VertexPersize, unsigned int ParamIndex);

		virtual void FuncAdditon(D3d11Render* pRender)=0;
		virtual void FillVerticeBuffer(DWORD Width, DWORD Height,void* FillBuffer) = 0;//填充void* pBuffer顶点缓冲
	};



	//图元拓扑参数
	struct DrawParam
	{
		ComPtr<ID3D11PixelShader> pPixelShader;//像素着色器
		ComPtr<ID3D11VertexShader> pVertexShader;//顶点着色器
		ComPtr<ID3D11InputLayout> pInputLayout;//输出布局
		D3D11_PRIMITIVE_TOPOLOGY Topology;//拓扑类型
	};

	class TaskTexture :public TaskBase
	{
	private:

		friend class D3d11Render;

		ComPtr<ID3D11Texture2D> pTexture2D;//纹理
		ComPtr<ID3D11ShaderResourceView> pSourse;//目标渲染视图
		TextureStyle Style;//纹理样式

	public:

		const float tFactor;//透明因子0-1
		const float eFactor;//曝光因子0-2，1代表正常曝光，0全暗，1全亮
		const float Scalling;//缩放率
		const unsigned int Width;//纹理宽度
		const unsigned int Height;//纹理高度
		const float Angle;//角度
		const FloatRect  tInvalidRect;//纹理有效区域
		const FloatPoint Pos;//坐标
		const RotatePoint rPoint;//旋转中心

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

		void PointRotating(float* X, float* Y, float RotateX, float RotateY, float AngleRadian);//旋转点

		void FillVerticeBuffer(DWORD Width, DWORD Heigth,void* FillBuffer) override;

		void FuncAdditon(D3d11Render* pRender)override;
	};
}

#endif //HS_DENUMSTRUCT
