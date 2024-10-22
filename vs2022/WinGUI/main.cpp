#include"inc/WinGUI.h"

using namespace HSLL;

void test(MessageType mType, WPARAM wParam, LPARAM lParam, void* This)
{
	return;
}


class TaskIcon :public WinTask
{
public :

	struct Point_Red
	{
		float x;
		float cos;
		float sin;
		double time = 0;
		double size =(std::rand() % 4 + 1) / 10.0;
		TaskTexture* pTask;
	};

	float counter = 0;
	ImgBGRA* pPoint;
	WinBase32* pWin;
	TaskTexture* pBack;
	TaskTexture* pLogo;
	std::list<Point_Red> v;

	TaskIcon(WinBase32* pWin) : pWin(pWin), WinTask::WinTask(true) {

		pPoint = LoadPng("Resourse//red_point.png");
		pBack = pWin->CreateTaskTexture("Resourse//staff_bg2_1.png", TextureStyle_Stretch, {}, { 0,0,1800,1100}, 1, 1, 1, 0, {});
		pLogo = pWin->CreateTaskTexture("Resourse//title_logo1.png", TextureStyle_Pos, {140,140}, { 0,0,673,458}, 1, 1, 1, 0, {});
		float Time = 0;
		while(Time<40000)
		{
			float angle = (std::rand() % 140+ 140) / 100.0;
			float startx = std::rand() % 1100 + 100;
			Point_Red PR{ startx, std::cos(angle),std::sin(angle),(double)(Time * 0.02) };
			PR.pTask = pWin->CreateTaskTexture(pPoint, TextureStyle_Pos, {}, { 0, 0, 10, 10 }, 1, 1, PR.size, 1, RotatePoint::TextureCenter);
			v.push_back(PR);	
			Time += 50;
		}
	};

	void Draw(double Interval, OpenRender* pRender) override
	{
		pRender->Draw(pBack);

		counter += Interval;
		DWORD NumPoint = 0;
		while (counter >=50)
		{
			counter -=50;
			NumPoint++;
		}
		for (int i = 0; i < NumPoint; i++)
		{
			float angle = (std::rand() % 147 + 157) / 100.0;
			float startx = std::rand() % 1100 + 100;
			Point_Red PR{ startx, std::cos(angle),std::sin(angle),0 };
			PR.pTask = pWin->CreateTaskTexture(pPoint, TextureStyle_Pos, {}, { 0, 0, 10, 10 }, 1, 1, PR.size, 1, RotatePoint::TextureCenter);
			v.push_back(PR);
		}	
		auto it = v.begin();
		while (it != v.end()) 
		{
			it->time +=Interval * 0.02;
			FloatPoint point = { it->x + it->time *it->cos,-it->time *it->sin + 720 };
			if (point.X < -10 || point.Y < -10||point.Y>730||point.X>1290) {
				delete it->pTask;
				it = v.erase(it); 
			}
			else 
			{
				it->pTask->Set_Pos(point);
				pRender->Draw(it->pTask);
				++it; 
			}
		}
		
		pRender->Draw(pLogo);
	}

	~TaskIcon()
	{
		delete pPoint;
		delete pBack;
		delete pLogo;
		while (v.size())
		{
			delete v.back().pTask;
			v.pop_back();
		}
	}
};


int CALLBACK WinMain(HINSTANCE hinstance, HINSTANCE hpreinstance, LPSTR lpcmdline, int ncmdshow)
{
	{
		WinMain32 wm(1280, 720, 200, 200);
		HICON hicon = CreateIcon("Resourse//cursor.png");
		HCURSOR  hCursor = CreateCursor("Resourse//cursor.png", 0, 0);
		wm.CreateWindow32(WinMainStyle_TitleBar_Sizeable,test, L"test", hicon);
		wm.SetCursor(hCursor);
		TaskIcon points(&wm);
		wm.AddTask(&points);
		wm.SetStrechParams(true, true);
		wm.Show();
		wm.WaitForClose();
		wm.Release();
	}
	return 0;
}