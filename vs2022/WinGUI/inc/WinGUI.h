#ifndef HS_WINGUI
#define HS_WINGUI

#include"WinEnumStruct.h"
#include<versionhelpers.h>
#include"Directx.h"
#include<dwmapi.h>
#include<ppl.h>

#pragma comment(lib,"Dwmapi.lib")

/*
32λ: ���ڴ����ɹ�Ϊ1
31λ: ӵ�б�����Ϊ1
30λ: �ɵ����߿��СΪ1
29λ: ��������Ϊ1
28λ: ȫ��Ϊ1
27λ: �������Ϊ1
26λ: ������С��Ϊ1
25λ: �����ö�Ϊ1
24λ: ������������Ϊ1
23λ: ����δ����ʱΪ1
22λ: ��������ק����Ϊ1
21λ: �������촰����Ϊ1
20λ: ���Ϸ���������Ϊ1
19λ: ���Ϸ���������Ϊ1
18λ: ���·���������Ϊ1
17λ: ���·���������Ϊ1
16λ: �Ϸ���������Ϊ1
15λ: �·���������Ϊ1
14λ: ����������Ϊ1
13λ: �ҷ���������Ϊ1
12λ: ������ʧȥ����ʱ��ͣ����Ϊ1
11λ: ����������ʱ���ִ���������Ϊ1
10λ: ����������ʱ��ͣ����Ϊ1
9λ: �������Ϊ1
8λ: ��������ر�Ϊ1
7λ: �����߳��˳�Ϊ1
6λ: ������Ϣ�߳��˳�Ϊ1
5λ: ��������Ϊ1
*/

namespace HSLL
{

#define WM_USER_WinCreate (WM_USER+0xfff+1)//���ڴ�����Ϣ
#define WM_USER_MainClose (WM_USER+0xfff+2)//������������Ϣ

	extern class WinTask;

	//����
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

		friend class WinChild32;//��Ԫ
		friend class WinMain32;//��Ԫ
		typedef  void  (*WinProc)(MessageType mType, WPARAM wParam, LPARAM lParam, void* This);//������Ϣ������

		struct WinCreateParam//���ڴ�������
		{
			DWORD dwStyle;
			LPCWSTR Title;
			WinBase32* pWind;
		};

		float posX;//����x
		float posY;//����y
		DWORD Width;//��
		DWORD Height;//��
		DWORD* FramesPerSecond;//����ˢ����

		WinProc Proc;//�û���Ϣ������
		WinBase32* pParent;//������
		D3d11Render* pDevice;//�豸
		std::list<WinBase32*> ChildList;//������

		HWND hWnd;//���ھ��
		DWORD State;//״̬	
		HCURSOR hCursor;//���
		HANDLE hMessageThread;//��Ϣ�����߳̾��
		WCHAR ClassName[20];//ע������
		CRITICAL_SECTION CriticalSection;//�ٽ���

		WinTask* pTask;//����
		OpenRender Render;//���û�������Ⱦ��

		static WORD RegisterCount;
		static DWORD WINAPI MessageThread(LPVOID pParam);//������Ϣ���߳�
		static LRESULT WINAPI WinProc32(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);//������Ϣ������

		WinBase32(DWORD Width, DWORD Height, float posX, float posY);//����

		BOOL CheckState(BYTE Bit);//��鴰��״̬λ

		WinBase32* GetMainWnd();//��ȡ������

		void CorrectPos();//�������������Լ�����

		void CorrectRgn();//�������������Լ�����

		void RemoveChild(WinBase32* pChild);//�Ƴ��Ӵ���

		void SetWinEnable(BOOL Enable);//���ô��ڼ�������

		void RegisterWinClass(HICON hIcon);//ע�ᴰ��

		void SetState(BYTE Bit, BYTE Binary);//���ô���״̬λ

		void AddToChildList(WinBase32* Child);//����Ӵ���

		void DrawWinTask(double Interval);//����

		virtual void DrawTask(double Interval) = 0;//�������

	public:


		void Show();//��ʾ����

		void Hide();//���ش���

		void Close();//�رմ��ڣ����� CreateWindow32��ɵ���

		void Move(float posX, float posY);	//�ƶ�����

		void SetCursor(HCURSOR hCursor);//���ù��(�Ӵ�����δ����ʱʹ�ø����ڹ��)

		void ReSize(DWORD Width, DWORD Height);	//resize����

		void MoveReSize(float posX, float posY, DWORD Width, DWORD Height);//�ƶ����Ĵ��ڴ�С

		BOOL AddTask(WinTask* pTask);//�������

		TaskTexture* CreateTaskTexture(LPCSTR PngPath, TextureStyle Style, FloatPoint Pos, FloatRect tInvalidRect,
			float eFactor, float tFactor, float Scalling, float Angle, RotatePoint RotatePoint);

		TaskTexture* CreateTaskTexture(ImgBGRA* pImage, TextureStyle Style, FloatPoint Pos, FloatRect tInvalidRect,
			float eFactor, float tFactor, float Scalling, float Angle, RotatePoint RotatePoint);

	};

	typedef WinBase32::OpenRender OpenRender;

	//������
	class WinMain32 :public WinBase32
	{
	private:

		friend class WinChild32;
		friend LRESULT WINAPI WinBase32::WinProc32(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);//������Ϣ������
		friend DWORD WINAPI WinBase32::MessageThread(LPVOID pIgnore);

		float AspectRatio;//���ڱ���
		float StrechCenter;//�������ĵ�
		HANDLE hClose;//���ڹر��ź�
		DWORD CornerRadius;//Բ�ǰ뾶
		FloatRect DraggingArea;//��ק��Ӧ����

		static DWORD WINAPI DrawThread(LPVOID pParam);//�߳�

		BOOL DrawRgn();//��������

		void DrawTask(double Interval) override;//�������

	public:

		WinMain32(DWORD Width, DWORD Height, float posX, float posY);//�����ڹ���

		void Release();//����WaitForClose��ɵ���

		void WaitForClose();//����CreateWindow32��ɵ���

		void SetRefreshrRate(DWORD Fps);//����ˢ����

		void SetDraggingArea(RECT Rect);//������Ӧ��ק����(���Ĵ��ڴ�Сʱ�ȱ�������)

		void SetWinTopMost(BOOL TopMost);//�ö�����

		void SetWinState(WinState State);//��ʾ������ʽ

		void SetStrechParams(BOOL SuspendDraw, BOOL FixedAspectRatio, float AspectRatio = -1);//���ô�������ʱ�Ĳ���

		void SetStrechEnable(BOOL LeftTop, BOOL RightTop, BOOL LeftBottom, BOOL RightBottom, BOOL Top, BOOL Bottom, BOOL Left, BOOL Right);//�����Ƿ������

		BOOL SetRoundedCorners(DWORD Radius);//���ô���Բ��(��������������ʱ����������Բ�����)

		BOOL CreateWindow32(WinMainStyle Style, WinProc Proc, LPCWSTR Title, HICON hIcon);//��������
	};


	//�Ӵ���
	class WinChild32 : public WinBase32
	{
	private:

		friend class WinMain32;//��Ԫ

		friend LRESULT WINAPI WinBase32::WinProc32(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);//������Ϣ������

		void DrawTask(double Interval) override;//�������

	public:

		WinChild32(DWORD  Width, DWORD Height, float posX, float posY);//�Ӵ��ڹ���

		BOOL CreateWindow32(WinBase32* pParent, WinProc Proc);	//��������
	};

	class WinTask
	{
		friend class WinBase32;

		bool TaskEnable;//�����Ƿ�������(���������ʱ�������GetTask����)

		bool RemoveTask;//�ó�ԱΪtrue���Ѿ����󶨵�����ʱ���������´λ������ڱ������Ƴ����������øó�ԱΪfalse

	public:

		WinTask(bool TaskEnable);//����

		//ÿ֡���Ӹú�����û�������,�����л�������Ϊ0��������, pWin:ָ����õĴ���(����FrameTask��,taskNumΪ0Ϊ��Ч֡����)
		virtual void Draw(double Interval, WinBase32::OpenRender* pRender) = 0;
	};
}
#endif // !1