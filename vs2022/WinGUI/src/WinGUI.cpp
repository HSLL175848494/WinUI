#include "../inc/WinGUI.h"

float NoClientWidth = 0;
float NoClientHeight = 0;

HSLL::WinTask::WinTask(bool TaskEnable) :TaskEnable(TaskEnable), RemoveTask(false) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////Callback
LRESULT WINAPI HSLL::WinBase32::WinProc32(HWND hWnd, UINT Id, WPARAM wParam, LPARAM lParam)//窗口消息处理函数
{

	WinMain32* mWin;
	WinBase32* bWin = (WinBase32*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

	if (!bWin)
	{
		if (Id == WM_CREATE)
		{
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
			((WinBase32*)(((LPCREATESTRUCT)lParam)->lpCreateParams))->hWnd = hWnd;
		}
		goto ProcSystem;
	}

	switch (Id)
	{
	case WM_MOUSEMOVE:

		TRACKMOUSEEVENT Time;
		Time = { sizeof(TRACKMOUSEEVENT) , TME_LEAVE | TME_HOVER ,hWnd,HOVER_DEFAULT };
		TrackMouseEvent(&Time);

		break;
	case WM_NCHITTEST://响应拉伸与拖动
	{
		if (wParam)//由子窗口发送的消息
			return wParam;


		mWin = (WinMain32*)bWin->GetMainWnd();//获得顶层窗口
		FloatPoint Pt = { (lParam & 0xffff) - mWin->posX, (lParam >> 16) - mWin->posY };//转化为主窗口坐标
		HRESULT PreResult = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
		HRESULT Result = PreResult;

		//如果命中客户区
		if (PreResult == HTCLIENT)
		{
			if (!mWin->CheckState(31) && mWin->CheckState(30))//主窗口没有标题栏并且主窗口响应拉伸(如果有标题栏或者不响应拉伸，则不需要处理拉伸逻辑)
			{
				DWORD CornerRadius = mWin->CornerRadius;
				float OffsetX = (Pt.X - mWin->Width / 2.0);
				float OffsetY = (Pt.Y - mWin->Height / 2.0);
				float TransX = abs(OffsetX) - (mWin->Width - 2 * CornerRadius) / 2.0;
				float TransY = abs(OffsetY) - (mWin->Height - 2 * CornerRadius) / 2.0;
				float sqDistance = TransX * TransX + TransY * TransY - CornerRadius * CornerRadius;

				if ((CornerRadius == 0 && TransX >= -10 && TransY >= -10) || (TransX >= 0 && TransY >= 0 && sqDistance >= (100 - 20.0 * CornerRadius) && sqDistance <= 0))
				{
					if (OffsetX < 0)
					{
						if (OffsetY < 0)
							Result = HTTOPLEFT;
						else
							Result = HTBOTTOMLEFT;
					}
					else
					{
						if (OffsetY < 0)
							Result = HTTOPRIGHT;
						else
							Result = HTBOTTOMRIGHT;
					}
				}
				else
				{
					if (CornerRadius - TransX <= 10)
					{
						if (OffsetX < 0)
							Result = HTLEFT;
						else
							Result = HTRIGHT;
					}
					else if (CornerRadius - TransY <= 10)
					{
						if (OffsetY < 0)
							Result = HTTOP;
						else
							Result = HTBOTTOM;
					}
				}
			}
		}

		bool Hit = 0;
		switch (Result)
		{
		case HTTOPLEFT:
			if (mWin->CheckState(20))
				Hit = true;
			break;
		case HTBOTTOMLEFT:
			if (mWin->CheckState(18))
				Hit = true;
			break;
		case HTTOPRIGHT:
			if (mWin->CheckState(19))
				Hit = true;
			break;
		case HTBOTTOMRIGHT:
			if (mWin->CheckState(17))
				Hit = true;
			break;
		case HTLEFT:
			if (mWin->CheckState(14))
				Hit = true;
			break;
		case HTRIGHT:
			if (mWin->CheckState(13))
				Hit = true;
			break;
		case HTTOP:
			if (mWin->CheckState(16))
				Hit = true;
			break;
		case HTBOTTOM:
			if (mWin->CheckState(15))
				Hit = true;
			break;
		}

		if (Hit)
		{
			if (!bWin->CheckState(24))//不是主窗口
			{
				SendMessageA(mWin->hWnd, WM_NCHITTEST, Result, 0);
				return HTTRANSPARENT;
			}
			else
				return Result;
		}
		else if (PreResult == HTCLIENT && mWin->CheckState(22) && mWin->DraggingArea.Left * mWin->Width <= Pt.X && mWin->DraggingArea.Right * mWin->Width >= Pt.X &&
			mWin->DraggingArea.Top * mWin->Height <= Pt.Y && mWin->DraggingArea.Bottom * mWin->Height >= Pt.Y)
		{
			if (!bWin->CheckState(24))//不是主窗口
			{
				SendMessageA(mWin->hWnd, WM_NCHITTEST, HTCAPTION, 0);
				return HTTRANSPARENT;
			}
			else
				return HTCAPTION;
		}

		return PreResult;//未命中
	}
	case WM_SETCURSOR://设置鼠标

		if (LOWORD(lParam) == HTCLIENT || LOWORD(lParam) == HTCAPTION)
		{
			::SetCursor(bWin->hCursor);
			return TRUE;
		}

		goto ProcSystem;
	case WM_SIZING://等比例拉伸

		if (bWin->CheckState(24) && bWin->CheckState(11))//保持窗口拉升比
		{
			mWin = (WinMain32*)bWin;
			RECT* Rect = (RECT*)lParam;
			float AspectRatio = mWin->AspectRatio;

			float  NoClientHeightTemp = 0, NoClientWidthTemp = 0;
			if (bWin->CheckState(31))
			{
				NoClientHeightTemp = NoClientHeight;
				NoClientWidthTemp = NoClientWidth;
			}
			switch (wParam)
			{

			case WMSZ_TOP:
			case WMSZ_BOTTOM:

				Rect->left = ceil(mWin->StrechCenter - (Rect->bottom - Rect->top - NoClientHeightTemp) * AspectRatio / 2 - NoClientWidthTemp / 2);
				Rect->right = mWin->StrechCenter + (Rect->bottom - Rect->top - NoClientHeightTemp) * AspectRatio / 2 + NoClientWidthTemp / 2;

				break;
			case WMSZ_TOPLEFT:
			case WMSZ_TOPRIGHT:

				Rect->top = ceil(Rect->bottom - (Rect->right - Rect->left - (float)NoClientWidthTemp) / AspectRatio - NoClientHeightTemp);

				break;
			case WMSZ_LEFT:
			case WMSZ_RIGHT:
			case WMSZ_BOTTOMRIGHT:
			case WMSZ_BOTTOMLEFT:

				Rect->bottom = Rect->top + (Rect->right - Rect->left - (float)NoClientWidthTemp) / AspectRatio + NoClientHeightTemp;

				break;
			}
			return TRUE;
		}

		goto ProcSystem;
	case WM_SIZE://伸缩边框时窗口更新大小

		if (wParam == SIZE_MINIMIZED)
		{
			bWin->SetState(26, 1);
			bWin->SetState(27, 0);
		}
		else
		{
			bWin->CorrectRgn();
			bWin->CorrectPos();

			if (wParam == SIZE_MAXIMIZED)
			{
				bWin->SetState(26, 0);
				bWin->SetState(27, 1);
			}
			else
			{
				if (bWin->CheckState(24))
				{
					mWin = (WinMain32*)bWin;
					mWin->DrawRgn();
				}
			}
		}

		return TRUE;
	case WM_MOVE:

		bWin->CorrectPos();

		goto ProcSystem;
	case WM_SETFOCUS:

		if (bWin->CheckState(12) && bWin->CheckState(24))
			bWin->SetState(9, 1);

		break;
	case WM_KILLFOCUS:

		if (bWin->CheckState(12) && bWin->CheckState(24))
			bWin->SetState(9, 0);

		break;
	case WM_ENTERSIZEMOVE://拉伸时是否暂停绘制

		if (bWin->CheckState(24))
		{
			mWin = (WinMain32*)bWin;
			if (bWin->CheckState(10))
				bWin->SetState(9, 0);
			RECT rect;
			GetWindowRect(bWin->hWnd, &rect);
			mWin->StrechCenter = (rect.left + rect.right) / 2.0;
		}

		goto ProcSystem;
	case WM_EXITSIZEMOVE:

		if (bWin->CheckState(10) && bWin->CheckState(24))
			bWin->SetState(9, 1);

		goto ProcSystem;
	case WM_DESTROY:

		bWin->SetState(8, 1);
		bWin->SetState(5, 1);

		if (bWin->CheckState(24))
		ReleaseSemaphore(((WinMain32*)bWin)->hClose, 1, 0);

		goto ProcSystem;
	}

	bWin->Proc((MessageType)Id, wParam, lParam, bWin);

ProcSystem:
	return DefWindowProc(hWnd, Id, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////MessageThread
DWORD WINAPI HSLL::WinBase32::MessageThread(LPVOID Param)//窗口消息接受线程
{
	MSG msg;
	WinMain32* pMain = (WinMain32*)Param;

	while (!pMain->CheckState(5))
	{
		if (GetMessageA(&msg, nullptr, 0, 0))
		{
			if (msg.message == WM_USER_WinCreate)
			{
				WinCreateParam* pParam = (WinCreateParam*)msg.wParam;

				RECT rc = { 0, 0, pParam->pWind->Width, pParam->pWind->Height };
				if (pParam->pWind->CheckState(31))
				{
					AdjustWindowRect(&rc, pParam->dwStyle, false);
					WinMain32* mWin = (WinMain32*)pParam->pWind;
					NoClientWidth = rc.right - rc.left - mWin->Width;
					NoClientHeight = rc.bottom - rc.top - mWin->Height;
				}

				//创建窗口
				HWND hWndParent = 0;
				if (pParam->pWind->pParent)
					hWndParent = pParam->pWind->pParent->hWnd;
				HWND hWnd = CreateWindowExW(0, pParam->pWind->ClassName, pParam->Title, pParam->dwStyle, pParam->pWind->posX, pParam->pWind->posY,
					rc.right - rc.left, rc.bottom - rc.top, hWndParent, 0, GetModuleHandleW(0), pParam->pWind);

				pParam->pWind->SetState(32, 1);
			}
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	pMain->SetState(6, 1);//消息线程退出
	ExitThread(0);
}


DWORD WINAPI HSLL::WinMain32::DrawThread(LPVOID pParam)//绘制线程
{
	WinMain32* pMain = (WinMain32*)pParam;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER StartTime, EndTime;
	QueryPerformanceFrequency(&Frequency);	// 获取系统时钟频率
	QueryPerformanceCounter(&StartTime);
	double FrameTime;

	while (!pMain->CheckState(5))
	{
		FrameTime = 1000.0 / (*pMain->FramesPerSecond);
		QueryPerformanceCounter(&EndTime);
		double ElapsedTime = (double)(EndTime.QuadPart - StartTime.QuadPart) * 1000 / (double)Frequency.QuadPart;
		QueryPerformanceCounter(&StartTime);

		if (ElapsedTime < FrameTime)//未到帧间隔
		{
			Sleep(FrameTime - ElapsedTime);
			pMain->DrawTask(FrameTime);
		}
		else
			pMain->DrawTask(ElapsedTime);
	}

	pMain->SetState(7, 1);//绘制线程退出
	ExitThread(0);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////WinBase
WORD  HSLL::WinBase32::RegisterCount = 0;

HSLL::WinBase32::WinBase32(DWORD Width, DWORD Height, float posX, float posY) :Render(this)//初始化
{
	this->posX = posX;
	this->posY = posY;
	this->Width = Width;
	this->Height = Height;
	hWnd = 0;
	State = 0;
	pTask = nullptr;
	pDevice = nullptr;
	pParent = nullptr;
	hMessageThread = INVALID_HANDLE_VALUE;
	hCursor = LoadCursorW(NULL, IDC_ARROW);
	SetState(9, 1);//允许绘制
}


void HSLL::WinBase32::SetState(BYTE Bit, BYTE Binary)//设置窗口状态位
{
	DWORD BitMask = 1 << (Bit - 1);

	if (Binary)
		State |= BitMask;
	else
		State &= ~BitMask;
}


BOOL HSLL::WinBase32::CheckState(BYTE Bit)//检查窗口状态位
{
	DWORD Num = 1 << (Bit - 1);
	return State & Num;
}


void HSLL::WinBase32::CorrectPos()//修正窗口坐标以及区域
{
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	posX = windowRect.left;
	posY = windowRect.top;

	if (!CheckState(24))
	{
		WinMain32* pMain = (WinMain32*)GetMainWnd();
		posX -= pMain->posX;
		posY -= pMain->posY;
	}
}


void HSLL::WinBase32::CorrectRgn()//修正窗口坐标以及区域
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	Width = rect.right - rect.left;
	Height = rect.bottom - rect.top;
}


void HSLL::WinBase32::AddToChildList(WinBase32* Child)//添加子节点
{
	EnterCriticalSection(&CriticalSection);
	ChildList.push_back(Child);
	LeaveCriticalSection(&CriticalSection);
	return;
}


HSLL::WinBase32* HSLL::WinBase32::GetMainWnd()//获取主窗口句柄
{
	WinBase32* pWin = this;
	while (pWin->pParent)
		pWin = pWin->pParent;
	return pWin;
}


void HSLL::WinBase32::RegisterWinClass(HICON hIcon)//注册窗口
{
	lstrcpyW(ClassName, L"Win32_00000");

	WORD Length = wcslen(ClassName);
	WORD WinNum = RegisterCount;
	WORD Count = 1;

	while (WinNum != 0)
	{
		ClassName[Length - Count] = (WCHAR)(WinNum % 10 + 48);
		WinNum /= 10; Count++;
	}

	RegisterCount++;

	//注册窗口
	HINSTANCE hInstance = GetModuleHandleW(nullptr);
	WNDCLASSW wc{};
	wc.lpfnWndProc = WinProc32;
	wc.hInstance = hInstance;
	wc.lpszClassName = ClassName;
	wc.hIcon = hIcon;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	RegisterClassW(&wc);
}


void HSLL::WinBase32::RemoveChild(WinBase32* pChild)//移除子窗口
{
	EnterCriticalSection(&CriticalSection);
	ChildList.remove(pChild);
	LeaveCriticalSection(&CriticalSection);
}


HSLL::TaskTexture* HSLL::WinBase32::CreateTaskTexture(LPCSTR PngPath, TextureStyle Style, FloatPoint Pos, FloatRect tInvalidRect, float eFactor, float tFactor, float Scalling, float Angle, RotatePoint RotatePoint)
{
	ImgBGRA* pImage = LoadPng(PngPath);

	if (!pImage || !pImage->Data || Scalling < 0 || tFactor < 0 || tFactor>1 || eFactor>2 || eFactor < 0)
		return nullptr;

	TaskTexture* pTask = new TaskTexture(pImage->Width, pImage->Height, Style, tFactor, eFactor, Scalling, Angle, Pos, RotatePoint, tInvalidRect);

	pDevice->CreateTexture(pImage, pTask);
	pDevice->CreateVertexBuffer(pTask, Width, Height);

	delete pImage;
	return pTask;
}

HSLL::TaskTexture* HSLL::WinBase32::CreateTaskTexture(ImgBGRA* pImage, TextureStyle Style, FloatPoint Pos, FloatRect tInvalidRect, float eFactor, float tFactor, float Scalling, float Angle, RotatePoint RotatePoint)
{
	if (!pImage || !pImage->Data || Scalling < 0 || tFactor < 0 || tFactor>1 || eFactor>2 || eFactor < 0)
		return nullptr;

	TaskTexture* pTask = new TaskTexture(pImage->Width, pImage->Height, Style, tFactor, eFactor, Scalling, Angle, Pos, RotatePoint, tInvalidRect);

	pDevice->CreateTexture(pImage, pTask);
	pDevice->CreateVertexBuffer(pTask, Width, Height);

	return  pTask;
}


void HSLL::WinBase32::SetWinEnable(BOOL Enable)//设置窗口激活属性
{
	if (CheckState(23) == Enable)
	{
		EnableWindow(hWnd, Enable);
		SetState(23, !Enable);
	}
}


void HSLL::WinBase32::Show()//显示窗口
{
	ShowWindow(hWnd, SW_SHOW);
	SetState(29, 0);
}


void HSLL::WinBase32::Hide()//隐藏窗口
{
	ShowWindow(hWnd, SW_HIDE);
	SetState(29, 1);
}


void HSLL::WinBase32::Close()
{
	if (!CheckState(8))//未发出请求
	{
		SetState(8, 1);//标记已发出关闭请求

		if (CheckState(24))//主窗口	
			SendMessageA(hWnd, WM_CLOSE, (WPARAM)this, 0);
		else
		{
			pParent->RemoveChild(this);

			if (!pParent->CheckState(8))
				SendMessageA(hWnd, WM_CLOSE, (WPARAM)this, 0);
		}

		for (auto pFree : ChildList)
		pFree->Close();
	}
}


void HSLL::WinBase32::Move(float posX, float posY)//移动窗口
{
	MoveWindow(hWnd, posX, posY, Width, Height, false);
}


void HSLL::WinBase32::ReSize(DWORD Width, DWORD Height)//设置窗口大小
{
	MoveWindow(hWnd, posX, posY, Width, Height, false);
}


void HSLL::WinBase32::MoveReSize(float posX, float posY, DWORD Width, DWORD Height)//移动设置窗口大小
{
	MoveWindow(hWnd, posX, posY, Width, Height, false);
}


void HSLL::WinBase32::SetCursor(HCURSOR hCursor)//设置光标
{
	this->hCursor = hCursor;
	PostMessageA(hWnd, WM_SETCURSOR, 0, 0);
}

BOOL HSLL::WinBase32::AddTask(WinTask* pTask)//添加任务
{
	EnterCriticalSection(&CriticalSection);
	this->pTask = pTask;
	LeaveCriticalSection(&CriticalSection);
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////WinMain32
HSLL::WinMain32::WinMain32(DWORD Width, DWORD Height, float posX, float posY)//父窗口构造
	:WinBase32(Width, Height, posX, posY)
{
	AspectRatio = Width * 1.0 / Height;
	NoClientWidth = 0;
	NoClientHeight = 0;
	SetState(12, 1);//窗口失去焦点时暂停绘制
}

void HSLL::WinMain32::Release()//请在窗口完全关闭后调用该函数
{
	while (!CheckState(7) || !CheckState(6));

	delete pDevice;
	delete FramesPerSecond;
	DeleteCriticalSection(&CriticalSection);
	UnregisterClassW(ClassName, GetModuleHandleW(nullptr));
}

void HSLL::WinMain32::WaitForClose()
{
	if(CheckState(25))
	WaitForSingleObject(hClose, INFINITE);
}


BOOL HSLL::WinMain32::CreateWindow32(WinMainStyle Style, WinProc Proc, LPCWSTR Title, HICON hIcon)//创建主窗口
{
	if (!Proc || Width == 0 || Height == 0)
		return false;

	SetProcessDPIAware();
	FramesPerSecond = new DWORD(60);

	//开启消息循环线程
	this->Proc = Proc;
	InitializeCriticalSection(&CriticalSection);
	hClose=CreateSemaphoreA(0, 0, 1, "Win_Close");
	hMessageThread = CreateThread(0, 0, MessageThread, this, 0, 0);
	SetThreadPriority(hMessageThread, THREAD_PRIORITY_ABOVE_NORMAL);

	RegisterWinClass(hIcon);//注册窗口类
	WinCreateParam wcParam{};
	wcParam.pWind = this;
	wcParam.Title = Title;

	//创建窗口
	if (Style & 0x2)//可调整大小的区域
	{
		SetState(30, 1);//可调整大小
		SetState(17, 1);
		SetState(13, 1);
		SetState(15, 1);
	}

	if (Style & 0x1)//有标题栏
	{
		wcParam.dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		if (Style & 0x2)
			wcParam.dwStyle |= WS_THICKFRAME;

		SetState(31, 1);
	}
	else
		wcParam.dwStyle |= WS_POPUP;

	SetState(24, 1);

	while (!PostThreadMessageA(GetThreadId(hMessageThread), WM_USER_WinCreate, (WPARAM)(&wcParam), 0));

	while (!CheckState(32));

	//创建绘图设备
	pDevice = new D3d11Render(hWnd, Width, Height);
	CreateThread(0, 0, DrawThread, this, 0, 0);
	return true;
}


BOOL HSLL::WinMain32::SetRoundedCorners(DWORD Radius)
{
	CornerRadius = Radius;
	return DrawRgn();
}


BOOL HSLL::WinMain32::DrawRgn()
{
	if (CornerRadius && !CheckState(31))
	{
		HRGN hRgn = CreateRoundRectRgn(0, 0, Width, Height, CornerRadius * 2, CornerRadius * 2);

		if (!SetWindowRgn(hWnd, hRgn, true))
			DeleteObject(hRgn);

		return true;
	}
	return false;
}


void HSLL::WinBase32::DrawWinTask(double Interval)//绘制
{
	if (pTask->RemoveTask)
	{
		pTask = nullptr;
		return;
	}

	if (pTask->TaskEnable)
		pTask->Draw(Interval, &Render);
}

void HSLL::WinMain32::DrawTask(double Interval)//主窗口任务绘制
{
	if (CheckState(29) || !CheckState(9))//不需要绘制
		return;

	pDevice->ClearView();
	pDevice->ReSize(Width, Height);

	EnterCriticalSection(&CriticalSection);//绘制时不允许有任务添加，窗口添加

	pDevice->SetViewPort({}, Width, Height);
	if (pTask)
		DrawWinTask(Interval);

	for (auto pChild : ChildList)
		pChild->DrawTask(Interval);

	LeaveCriticalSection(&CriticalSection);
	pDevice->Present();
}


void HSLL::WinMain32::SetWinTopMost(BOOL TopMost)//窗口置顶
{
	if (TopMost)
	{
		SetState(25, 1);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetState(25, 0);
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}


void HSLL::WinMain32::SetWinState(WinState State)
{
	switch (State)
	{
	case HSLL::WinState_Normal:

		if (CheckState(26) || CheckState(27))
			SendMessageA(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		else if (CheckState(28))
		{
			if (CheckState(31))
			{
				LONG Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
				if (CheckState(30))
					Style |= WS_THICKFRAME;
				SetWindowLongA(hWnd, GWL_STYLE, Style);
			}

			SetWindowPos(hWnd, HWND_NOTOPMOST, posX, posY, Width, Height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}

		break;

	case HSLL::WinState_MiniMize:

		if (!CheckState(26))
			ShowWindow(hWnd, SW_MINIMIZE);

		break;
	case HSLL::WinState_FullScreen:

		if (!CheckState(28))
		{
			WORD screenWidth = GetSystemMetrics(SM_CXSCREEN);
			WORD screenHeight = GetSystemMetrics(SM_CYSCREEN);

			if (CheckState(31))
				SetWindowLongA(hWnd, GWL_STYLE, WS_POPUPWINDOW);
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			SetState(28, 1);
		}

		break;
	}
}


void HSLL::WinMain32::SetRefreshrRate(DWORD Fps)//设置刷新率
{
	*FramesPerSecond = Fps;
}


void HSLL::WinMain32::SetDraggingArea(RECT Rect)//响应区域
{
	if (Rect.left >= Rect.right || Rect.bottom <= Rect.top)
		return;

	DraggingArea = { (float)Rect.left / Width ,(float)Rect.top / Height,(float)Rect.right / Height , (float)Rect.bottom / Height };

	SetState(22, 1);
}


void HSLL::WinMain32::SetStrechEnable(BOOL LeftTop, BOOL RightTop,
	BOOL LeftBottom, BOOL RightBottom, BOOL Top, BOOL Bottom, BOOL Left, BOOL Right)//设置是否可拉伸
{
	SetState(20, LeftTop);
	SetState(19, RightTop);
	SetState(18, LeftBottom);
	SetState(17, RightBottom);
	SetState(16, Top);
	SetState(15, Bottom);
	SetState(14, Left);
	SetState(13, Right);
}


void HSLL::WinMain32::SetStrechParams(BOOL SuspendDraw, BOOL FixedAspectRatio, float AspectRatio)//设置窗口拉升时的参数
{
	if (FixedAspectRatio && AspectRatio != -1)
		this->AspectRatio = AspectRatio;
	SetState(11, FixedAspectRatio);
	SetState(10, SuspendDraw);
}

void HSLL::WinChild32::DrawTask(double Interval)//任务绘制
{
	if (CheckState(29) || !CheckState(9))//不需要绘制
		return;

	if (pTask)
	{
		pDevice->SetViewPort({ (LONG)posX ,(LONG)posY }, Width, Height);
		DrawWinTask(Interval);
	}

	for (auto pChild : ChildList)
		pChild->DrawTask(Interval);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////WinChild32
HSLL::WinChild32::WinChild32(DWORD Width, DWORD Height, float posX, float posY)//子窗口构造
	:WinBase32(Width, Height, posX, posY) {}


BOOL HSLL::WinChild32::CreateWindow32(WinBase32* pParent, WinProc Proc)//创建窗口
{
	if (!Proc || Width == 0 || Height == 0 || !pParent)
		return false;

	this->Proc = Proc;
	this->pParent = pParent;
	hCursor = pParent->hCursor;
	pDevice = pParent->pDevice;
	FramesPerSecond = pParent->FramesPerSecond;
	lstrcpyW(ClassName, pParent->ClassName);
	CriticalSection = pParent->CriticalSection;
	hMessageThread = pParent->hMessageThread;

	WinCreateParam wcParam{};
	wcParam.pWind = this;
	wcParam.dwStyle |= WS_CHILD;

	while (!PostThreadMessageA(GetThreadId(hMessageThread), WM_USER_WinCreate, (WPARAM)(&wcParam), 0));
	while (!CheckState(32));

	pParent->AddToChildList(this);
	return true;
}


void HSLL::WinBase32::OpenRender::Draw(TaskBase* pTask)
{
	if (pTask->NeedUpdate)
	{
		pWin->pDevice->CreateVertexBuffer(pTask, pWin->Width, pWin->Height);
		pTask->NeedUpdate = false;//更新纹理或者重定位纹理坐标
	}
	pWin->pDevice->DrawTask(pTask);
}