#ifndef HS_LOADIMAGE
#define HS_LOADIMAGE

#define _CRT_SECURE_NO_WARNINGS

//inc
#include"Png.h"
#include<Windows.h>

namespace HSLL
{
	//rgba标准颜色
	struct ColorRGBA
	{
		BYTE R;
		BYTE G;
		BYTE B;
		BYTE A;

		//透明
		static const ColorRGBA TransParent;
		//黑色
		static const ColorRGBA Black;
		//红色
		static const ColorRGBA Red;
		//绿色
		static const ColorRGBA Green;
		//蓝色
		static const ColorRGBA Blue;
		//白色
		static const ColorRGBA White;
		//黄色
		static const ColorRGBA Yellow;
		//紫色
		static const ColorRGBA Purple;
		//青色
		static const ColorRGBA Cyan;

	};

	//图片数据结构
	struct ImgBGRA
	{
		DWORD Width;
		DWORD Height;
		BYTE* Data;
		~ImgBGRA();
	};

	ImgBGRA* LoadPng(LPCSTR FilePath);	//载入png图片

	ImgBGRA* LoadPng(BYTE* Data, DWORD size);//内存载入

	ImgBGRA* LoadBmp(BYTE* Buffer, DWORD Size);

	ImgBGRA* LoadBmp(LPCSTR FilePath);	//加载bmp图片，创建数据对象

	HICON CreateIcon(LPCSTR PathPng);//创建Icon

	HICON CreateIcon(BYTE* rgbaData, DWORD width, DWORD height);//创建Icon

	HCURSOR CreateCursor(LPCSTR PathPng, DWORD  xHotspot, DWORD yHotspot);//创建CurSor

	HCURSOR CreateCursor(BYTE* rgbaData, DWORD  width, DWORD  height, DWORD  xHotspot, DWORD yHotspot);//创建CurSor
}

#endif // HS_LOADIMAGE