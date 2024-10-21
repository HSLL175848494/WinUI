#ifndef HS_WINENUM32

#include<Windows.h>

namespace HSLL
{
	enum WinMainStyle//����������
	{
		WinMainStyle_Normal=0,//��ͨ
		WinMainStyle_TitleBar=1,//���б�����
		WinMainStyle_Sizeable=2,//�ɵ�����С
		WinMainStyle_TitleBar_Sizeable=3,//���б������ɵ�����С
	};

	enum WinState//����״̬
	{
		WinState_Normal,
		WinState_MaxMize,
		WinState_MiniMize,
		WinState_FullScreen
	};
	

	enum MessageType//��Ϣ����
	{
		MessageType_KeyDown = WM_KEYDOWN,
		MessageType_KeyUp = WM_KEYUP,
		MessageType_LeftButtonDown = WM_LBUTTONDOWN,
		MessageType_LeftButtonUp = WM_LBUTTONUP,
		MessageType_RightButtonDown = WM_RBUTTONDOWN,
		MessageType_RightButtonUp = WM_RBUTTONUP,
		MessageType_MouseRDoubleClick = WM_RBUTTONDBLCLK,
		MessageType_MouseLDoubleClick = WM_LBUTTONDBLCLK,
		MessageType_MouseHover = WM_MOUSEHOVER,
		MessageType_MouseMove = WM_MOUSEMOVE,
		MessageType_MouseLeave = WM_MOUSELEAVE,
		MessageType_MouseEnter = WM_USER + 0xfff + 4,
		MessageType_GetFocus = WM_USER + 0xfff + 5,//��ý���
		MessageType_LoseFocus = WM_USER + 0xfff + 6,//ʧȥ����
	};
}

#define HS_WINENUM32
#endif // DEBUG