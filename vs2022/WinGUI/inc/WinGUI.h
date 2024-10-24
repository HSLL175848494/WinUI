#ifndef HS_WINGUI
#define HS_WINGUI

#include"WinEnumStruct.h"
#include<versionhelpers.h>
#include"Directx.h"
#include<dwmapi.h>
#include<ppl.h>

#pragma comment(lib,"Dwmapi.lib")

/*
32位: 窗口创建成功为1
31位: 拥有标题栏为1
30位: 可调整边框大小为1
29位: 窗口隐藏为1
28位: 全屏为1
27位: 窗口最大化为1
26位: 窗口最小化为1
25位: 窗口置顶为1
24位: 窗口是主窗口为1
23位: 窗口未激活时为1
22位: 已设置拖拽区域为1
21位: 处于拉伸窗口中为1
20位: 左上方允许拉伸为1
19位: 右上方允许拉伸为1
18位: 左下方允许拉伸为1
17位: 右下方允许拉伸为1
16位: 上方允许拉伸为1
15位: 下方允许拉伸为1
14位: 左方允许拉伸为1
13位: 右方允许拉伸为1
12位: 主窗口失去焦点时暂停绘制为1
11位: 主窗口拉伸时保持窗口拉升比为1
10位: 主窗口拉伸时暂停绘制为1
9位: 允许绘制为1
8位: 窗口请求关闭为1
7位: 绘制线程退出为1
6位: 窗口消息线程退出为1
5位: 窗口销毁为1
*/

namespace HSLL
{

#define WM_USER_WinCreate (WM_USER+0xfff+1)//窗口创建消息
#define WM_USER_MainClose (WM_USER+0xfff+2)//主窗口销毁消息

	extern class WinTask;

	//基类
	class WinBase32
	{
	public:

		class OpenRender
		{
		private:

			WinBase32* pWin;

		public:

			OpenRender(WinBase32* pWin) :pWin(pWin) {};

			void Draw(TaskBase* pTask);
		};

	private:

		friend class WinChild32;//友元
		friend class WinMain32;//友元
		typedef  void  (*WinProc)(MessageType mType, WPARAM wParam, LPARAM lParam, void* This);//窗口消息处理函数

		struct WinCreateParam//窗口创建参数
		{
			DWORD dwStyle;
			LPCWSTR Title;
			WinBase32* pWind;
		};

		float posX;//坐标x
		float posY;//坐标y
		DWORD Width;//宽
		DWORD Height;//高
		DWORD* FramesPerSecond;//窗口刷新率

		WinProc Proc;//用户消息处理函数
		WinBase32* pParent;//父窗口
		D3d11Render* pDevice;//设备
		std::list<WinBase32*> ChildList;//子链表

		HWND hWnd;//窗口句柄
		DWORD State;//状态	
		HCURSOR hCursor;//光标
		HANDLE hMessageThread;//消息接收线程句柄
		WCHAR ClassName[20];//注册类名
		CRITICAL_SECTION CriticalSection;//临界区

		WinTask* pTask;//绘制
		OpenRender Render;//对用户开放渲染器

		static WORD RegisterCount;
		static DWORD WINAPI MessageThread(LPVOID pParam);//窗口消息收线程
		static LRESULT WINAPI WinProc32(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);//窗口消息处理函数

		WinBase32(DWORD Width, DWORD Height, float posX, float posY);//构造

		BOOL CheckState(BYTE Bit);//检查窗口状态位

		WinBase32* GetMainWnd();//获取主窗口

		void CorrectPos();//修正窗口坐标以及区域

		void CorrectRgn();//修正窗口坐标以及区域

		void RemoveChild(WinBase32* pChild);//移除子窗口

		void SetWinEnable(BOOL Enable);//设置窗口激活属性

		void RegisterWinClass(HICON hIcon);//注册窗口

		void SetState(BYTE Bit, BYTE Binary);//设置窗口状态位

		void AddToChildList(WinBase32* Child);//添加子窗口

		void DrawWinTask(double Interval);//绘制

		virtual void DrawTask(double Interval) = 0;//任务绘制

	public:


		void Show();//显示窗口

		void Hide();//隐藏窗口

		void Close();//关闭窗口，调用 CreateWindow32后可调用

		void Move(float posX, float posY);	//移动窗口

		void SetCursor(HCURSOR hCursor);//设置光标(子窗口在未设置时使用父窗口光标)

		void ReSize(DWORD Width, DWORD Height);	//resize窗口

		void MoveReSize(float posX, float posY, DWORD Width, DWORD Height);//移动更改窗口大小

		BOOL AddTask(WinTask* pTask);//添加任务

		TaskTexture* CreateTaskTexture(LPCSTR PngPath, TextureStyle Style, FloatPoint Pos, FloatRect tInvalidRect,
			float eFactor, float tFactor, float Scalling, float Angle, RotatePoint RotatePoint);

		TaskTexture* CreateTaskTexture(ImgBGRA* pImage, TextureStyle Style, FloatPoint Pos, FloatRect tInvalidRect,
			float eFactor, float tFactor, float Scalling, float Angle, RotatePoint RotatePoint);

	};

	typedef WinBase32::OpenRender OpenRender;

	//主窗口
	class WinMain32 :public WinBase32
	{
	private:

		friend class WinChild32;
		friend LRESULT WINAPI WinBase32::WinProc32(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);//窗口消息处理函数
		friend DWORD WINAPI WinBase32::MessageThread(LPVOID pIgnore);

		float AspectRatio;//窗口比例
		float StrechCenter;//拉伸中心点
		HANDLE hClose;//窗口关闭信号
		DWORD CornerRadius;//圆角半径
		FloatRect DraggingArea;//拖拽响应区域

		static DWORD WINAPI DrawThread(LPVOID pParam);//线程

		BOOL DrawRgn();//绘制区域

		void DrawTask(double Interval) override;//任务绘制

	public:

		WinMain32(DWORD Width, DWORD Height, float posX, float posY);//主窗口构造

		void Release();//调用WaitForClose后可调用

		void WaitForClose();//调用CreateWindow32后可调用

		void SetRefreshrRate(DWORD Fps);//设置刷新率

		void SetDraggingArea(RECT Rect);//设置响应拖拽区域(更改窗口大小时等比例修正)

		void SetWinTopMost(BOOL TopMost);//置顶窗口

		void SetWinState(WinState State);//显示窗口样式

		void SetStrechParams(BOOL SuspendDraw, BOOL FixedAspectRatio, float AspectRatio = -1);//设置窗口拉升时的参数

		void SetStrechEnable(BOOL LeftTop, BOOL RightTop, BOOL LeftBottom, BOOL RightBottom, BOOL Top, BOOL Bottom, BOOL Left, BOOL Right);//设置是否可拉伸

		BOOL SetRoundedCorners(DWORD Radius);//设置窗口圆角(当窗口是主窗口时与标题栏属性不兼容)

		BOOL CreateWindow32(WinMainStyle Style, WinProc Proc, LPCWSTR Title, HICON hIcon);//创建窗口
	};


	//子窗口
	class WinChild32 : public WinBase32
	{
	private:

		friend class WinMain32;//友元

		friend LRESULT WINAPI WinBase32::WinProc32(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);//窗口消息处理函数

		void DrawTask(double Interval) override;//任务绘制

	public:

		WinChild32(DWORD  Width, DWORD Height, float posX, float posY);//子窗口构造

		BOOL CreateWindow32(WinBase32* pParent, WinProc Proc);	//创建窗口
	};

	class WinTask
	{
		friend class WinBase32;

		bool TaskEnable;//任务是否参与绘制(不参与绘制时不会调用GetTask函数)

		bool RemoveTask;//该成员为true且已经被绑定到窗口时，任务将在下次绘制周期被窗口移除并重新设置该成员为false

	public:

		WinTask(bool TaskEnable);//构造

		//每帧将从该函数获得绘制任务,返回中绘制数量为0将不绘制, pWin:指向调用的窗口(返回FrameTask中,taskNum为0为无效帧任务)
		virtual void Draw(double Interval, WinBase32::OpenRender* pRender) = 0;
	};
}
#endif // !1