#ifndef HS_LOADIMAGE
#define HS_LOADIMAGE

#define _CRT_SECURE_NO_WARNINGS

//inc
#include"Png.h"
#include<Windows.h>

namespace HSLL
{
	//rgba��׼��ɫ
	struct ColorRGBA
	{
		BYTE R;
		BYTE G;
		BYTE B;
		BYTE A;

		//͸��
		static const ColorRGBA TransParent;
		//��ɫ
		static const ColorRGBA Black;
		//��ɫ
		static const ColorRGBA Red;
		//��ɫ
		static const ColorRGBA Green;
		//��ɫ
		static const ColorRGBA Blue;
		//��ɫ
		static const ColorRGBA White;
		//��ɫ
		static const ColorRGBA Yellow;
		//��ɫ
		static const ColorRGBA Purple;
		//��ɫ
		static const ColorRGBA Cyan;

	};

	//ͼƬ���ݽṹ
	struct ImgBGRA
	{
		DWORD Width;
		DWORD Height;
		BYTE* Data;
		~ImgBGRA();
	};

	ImgBGRA* LoadPng(LPCSTR FilePath);	//����pngͼƬ

	ImgBGRA* LoadPng(BYTE* Data, DWORD size);//�ڴ�����

	ImgBGRA* LoadBmp(BYTE* Buffer, DWORD Size);

	ImgBGRA* LoadBmp(LPCSTR FilePath);	//����bmpͼƬ���������ݶ���

	HICON CreateIcon(LPCSTR PathPng);//����Icon

	HICON CreateIcon(BYTE* rgbaData, DWORD width, DWORD height);//����Icon

	HCURSOR CreateCursor(LPCSTR PathPng, DWORD  xHotspot, DWORD yHotspot);//����CurSor

	HCURSOR CreateCursor(BYTE* rgbaData, DWORD  width, DWORD  height, DWORD  xHotspot, DWORD yHotspot);//����CurSor
}

#endif // HS_LOADIMAGE