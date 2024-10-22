#ifndef HS_D3D11DEVICE
#define HS_D3D11DEVICE

//inc
#include"../resource.h"
#include"DirectxEnumStruct.h"
#include"LoadResourse.h"
#include<vector>
//lib
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

//定义错误Debug宏
#if defined(_DEBUG)

#define DIRECTERROR(hr) HSLL::GetErrorString(hr,__FILE__,__LINE__)

#else

#define DIRECTERROR(hr) hr

#endif

namespace HSLL
{
	/*

	D3d11Render::state

	32位:1表示使用的是纹理渲染模式,0表示多边形或者直线

	*/

	//获取错误字符串(弹出窗口)
	void GetErrorString(HRESULT hResult, LPCSTR File, DWORD Line);

	class D3d11Render
	{
	private:

		int Index;//状态
		HWND hWnd;//窗口句柄
		DWORD Width;//宽
		DWORD Height;//高
		std::vector<DrawParam> DrawParams;//绘制参数

		void CreateSwapChain();//创建交换链
		void CreateSampler();//创建采样器
		void CreateBlendState();//创建混合状态
		void CreateConstantBuffers();//创建常量缓冲
		void CreateRenderTarget();//创建目标渲染视图
		void CreateShaders();//创建着色器
		void CreateRasterizerState();//设置光栅器
		void RecreateBuffer();//重新创建一系列参数以适应窗口大小
		BOOL HasDedicatedGPU();//是否拥有独立显卡设备

	public:

		ComPtr<ID3D11Buffer> FactorBuffers_T;
		ComPtr<ID3D11Device> pDevice;//设备
		ComPtr<IDXGISwapChain> pSwap;	//交换链
		ComPtr<ID3D11DeviceContext> pContext;	//设备上下文	
		ComPtr<ID3D11RenderTargetView> pRenderTargetView;	//后台缓冲目标视图

		D3d11Render(HWND hWnd, DWORD Width, DWORD Height);//构造

		void Present();//呈现

		void ClearView();//清理渲染视图

		void SetFactors_T(float eFactor, float tFactor);//设置因子

		void DrawTask(TaskBase* pTask);//绘制任务

		void ReSize(DWORD Width, DWORD Height);//更新窗口大小

		void SetViewPort(POINT Pos, DWORD Width, DWORD Height);//设置视口大小

		void CreateTexture(ImgBGRA* pImage, TaskTexture* pTask);//创建纹理

		void CreateVertexBuffer(TaskBase* pTask, DWORD Width, DWORD Height);//创建顶点缓冲
	};
}

#endif // !HS_D3D11DEVICE
