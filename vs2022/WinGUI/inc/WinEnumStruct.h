#ifndef HS_WINENUM32

#include<Windows.h>

namespace HSLL
{
	enum WinMainStyle//主窗口属性
	{
		WinMainStyle_Normal=0,//普通
		WinMainStyle_TitleBar=1,//含有标题栏
		WinMainStyle_Sizeable=2,//可调整大小
		WinMainStyle_TitleBar_Sizeable=3,//含有标题栏可调整大小
	};

	enum WinState//窗口状态
	{
		WinState_Normal,
		WinState_MaxMize,
		WinState_MiniMize,
		WinState_FullScreen
	};
	

	enum MessageType//消息类型
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
		MessageType_GetFocus = WM_USER + 0xfff + 5,//获得焦点
		MessageType_LoseFocus = WM_USER + 0xfff + 6,//失去焦点
	};
}

#define HS_WINENUM32
#endif // DEBUG