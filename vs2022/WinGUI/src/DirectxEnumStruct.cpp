#include"../inc/DirectxEnumStruct.h"
#include"../inc/Directx.h"

const HSLL::RotatePoint  HSLL::RotatePoint::TextureCenter = { D3D11_FLOAT32_MAX,0 };
const HSLL::RotatePoint  HSLL::RotatePoint::LeftCenter = { D3D11_FLOAT32_MAX,1 };
const HSLL::RotatePoint  HSLL::RotatePoint::RightCenter = { D3D11_FLOAT32_MAX,2 };
const HSLL::RotatePoint  HSLL::RotatePoint::TopCenter = { D3D11_FLOAT32_MAX,3 };
const HSLL::RotatePoint  HSLL::RotatePoint::BottomCenter = { D3D11_FLOAT32_MAX,4 };
const HSLL::RotatePoint  HSLL::RotatePoint::LeftTop = { D3D11_FLOAT32_MAX,5 };
const HSLL::RotatePoint  HSLL::RotatePoint::RightTop = { D3D11_FLOAT32_MAX,6 };
const HSLL::RotatePoint  HSLL::RotatePoint::LeftBottom = { D3D11_FLOAT32_MAX,7 };
const HSLL::RotatePoint  HSLL::RotatePoint::RightBottom = { D3D11_FLOAT32_MAX,8 };

HSLL::TaskBase::TaskBase(unsigned int VerticesNum, unsigned int VertexPersize, unsigned int ParamIndex)
	:NeedUpdate(false), VerticesNum(VerticesNum), VertexPersize(VertexPersize), ParamIndex(ParamIndex) {}

bool HSLL::RotatePoint::operator==(const RotatePoint& Other)
{
	return X == Other.X && Y == Other.Y;
}

void HSLL::TaskTexture::Set_tFactor(float tFactor)
{
	if (tFactor >= 0&&tFactor<=1)
	*((float*)&this->tFactor) = tFactor;
}

void HSLL::TaskTexture::Set_eFactor(float eFactor)
{
	if (eFactor >= 0 && eFactor <= 2)
		*((float*)&this->eFactor) = eFactor;
}

void HSLL::TaskTexture::Set_Scalling(float Scalling)
{
	if (Scalling >= 0)
	{
		*((float*)&this->Scalling) = Scalling;
		NeedUpdate = true;
	}
}

void HSLL::TaskTexture::Set_Angle(float Angle)
{
	*((float*)&this->Angle) = Angle;
	NeedUpdate = true;
}

void HSLL::TaskTexture::Set_Pos(FloatPoint Pos)
{
	*((FloatPoint*)&this->Pos) = Pos;
	NeedUpdate = true;
}

void HSLL::TaskTexture::Set_RotatePoint(RotatePoint rPoint)
{
	*((RotatePoint*)&this->rPoint) = rPoint;
	NeedUpdate = true;
}

void HSLL::TaskTexture::Set_tInvalidRect(FloatRect tInvalidRect)
{
	*((FloatRect*)&this->tInvalidRect) = tInvalidRect;
	NeedUpdate = true;
}

void HSLL::TaskTexture::PointRotating(float* X, float* Y, float RotateX, float RotateY, float AngleRadian)
{
	float TempX = *X - RotateX;
	float TempY = *Y - RotateY;
	*X = RotateX + (TempX * cos(AngleRadian) - TempY * sin(AngleRadian));
	*Y = RotateY + (TempX * sin(AngleRadian) + TempY * cos(AngleRadian));
}

void HSLL::TaskTexture::FillVerticeBuffer(DWORD Width,DWORD Height,void* FillBuffer)
{
	VertexTexture* Vertices = (VertexTexture*)FillBuffer;

	//坐标点为左上和右下
	float X1, Y1, X2, Y2, U1, V1, U2, V2, textureAspectRatio, windowAspectRatio;

	//纹理长度
	FloatRect tInvalidRect = this->tInvalidRect;
	float tInvalidWidth = tInvalidRect.Right - tInvalidRect.Left;
	float tInvalidHeight = tInvalidRect.Bottom - tInvalidRect.Top;

	//计算纹理坐标
	switch (this->Style)
	{
	case HSLL::TextureStyle_Pos://计算纹理坐标

		X1 = Pos.X ;
		X2 = X1 + tInvalidWidth * Scalling;
		Y1 = Pos.Y;
		Y2 = Y1 + tInvalidHeight * Scalling;

		break;
	case HSLL::TextureStyle_Tile://忽略pos

		X1 = 0;
		X2 = X1 + tInvalidWidth * Scalling;
		Y1 = 0;
		Y2 = Y1 + tInvalidHeight * Scalling;

		break;
	case HSLL::TextureStyle_Center://忽略pos

		X1 =(Width-tInvalidWidth*Scalling) / 2.0;
		X2 = X1 + tInvalidWidth * Scalling;
		Y1 = (Height- tInvalidHeight*Scalling) / 2.0;
		Y2 = Y1 + tInvalidHeight * Scalling;

		break;
	case HSLL::TextureStyle_Stretch:////忽略pos,拉伸,忽略Scalling

		X1 = 0;
		X2 = Width;
		Y1 =0;
		Y2 = Height;

		break;
	case HSLL::TextureStyle_Stretch_Align: // 忽略pos，居中对齐拉伸，忽略Scalling

		textureAspectRatio = tInvalidWidth / tInvalidHeight;
		windowAspectRatio = Width*1.0/Height;

		if (textureAspectRatio > windowAspectRatio) // 纹理宽高比大于窗口宽高比
		{
			float newHeight = Width/ textureAspectRatio;
			Y1 = (Height - newHeight) / 2;
			Y2 = Y1 + newHeight;
			X1 = 0;
			X2 = Width;
		}
		else // 纹理宽高比小于或等于窗口宽高比
		{
			float newWidth = Height * textureAspectRatio;
			X1 = (Width - newWidth) / 2;
			X2 = X1 + newWidth;
			Y1 = 0;
			Y2 = Height;
		}
		break;
	default:
		break;
	}

	//填充坐标
	Vertices[0] = { X1,Y1 };
	Vertices[1] = { X1,Y2 };
	Vertices[2] = { X2,Y1 };
	Vertices[3] = { X2,Y2 };

	if (this->Angle)//旋转变化
	{
		float Angle = fmod(this->Angle, 360);

		RotatePoint  Rt = rPoint;

		if (Rt == RotatePoint::TextureCenter)//纹理中心旋转
		{
			Rt.X = (X1 + X2) / 2.0;
			Rt.Y = (Y1 + Y2) / 2.0;
		}
		else if (Rt == RotatePoint::LeftTop)
		{
			Rt.X = X1;
			Rt.Y = Y1;
		}
		else if (Rt == RotatePoint::RightTop)
		{
			Rt.X = X2;
			Rt.Y = Y1;
		}
		else if (Rt == RotatePoint::LeftBottom)
		{
			Rt.X = X1;
			Rt.Y = Y2;
		}
		else if (Rt == RotatePoint::RightBottom)
		{
			Rt.X = X2;
			Rt.Y = Y2;
		}
		else if (Rt == RotatePoint::TopCenter)
		{
			Rt.X = (X1 + X2) / 2.0;
			Rt.Y = Y1;
		}
		else if (Rt == RotatePoint::BottomCenter)
		{
			Rt.X = (X1 + X2) / 2.0;
			Rt.Y = Y2;
		}
		else if (Rt == RotatePoint::LeftCenter)
		{
			Rt.X = X1;
			Rt.Y = (Y1 + Y2) / 2.0;
		}
		else if (Rt == RotatePoint::RightCenter)
		{
			Rt.X = X2;
			Rt.Y = (Y1 + Y2) / 2.0;
		}


		//旋转变换
		float AngleRadian = Angle * HSLL_MATH_PI / 180.0f;

		for (BYTE i = 0; i < 4; i++)
			PointRotating(&Vertices[i].X, &Vertices[i].Y, Rt.X, Rt.Y, AngleRadian);
	}

	//获取UV坐标,起点为纹理右上角
	U1 = tInvalidRect.Left / this->Width;
	U2 = tInvalidRect.Right / this->Width;
	V1 = tInvalidRect.Top / this->Height;
	V2 = tInvalidRect.Bottom / this->Height;
	Vertices[0].U = U1; Vertices[0].V = V1;
	Vertices[1].U = U1; Vertices[1].V = V2;
	Vertices[2].U = U2; Vertices[2].V = V1;
	Vertices[3].U = U2; Vertices[3].V = V2;

	//x,y坐标归一化
	for (BYTE i = 0; i < 4; i++)
	{
		Vertices[i].X = (2 * Vertices[i].X /Width) - 1;
		Vertices[i].Y = 1 - (2 * Vertices[i].Y /Height);
	}
}

void HSLL::TaskTexture::FuncAdditon(D3d11Render* pRender)
{
	pRender->pContext->PSSetShaderResources(0, 1, pSourse.GetAddressOf());
	pRender->SetFactors_T(eFactor,tFactor);
}

HSLL::TaskTexture::TaskTexture(unsigned int Width, unsigned int Height, TextureStyle tStyle, float tFactor, float eFactor, float Scalling,
	float Angle, FloatPoint Pos, RotatePoint rPoint, FloatRect tInvalidRect)
	: Width(Width), Height(Height), Style(tStyle), tFactor(tFactor), eFactor(eFactor),
	Scalling(Scalling), Angle(Angle), Pos(Pos), rPoint(rPoint),
	tInvalidRect(tInvalidRect), TaskBase::TaskBase(4,sizeof(VertexTexture),0) {}