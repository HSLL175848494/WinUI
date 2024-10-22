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

//�������Debug��
#if defined(_DEBUG)

#define DIRECTERROR(hr) HSLL::GetErrorString(hr,__FILE__,__LINE__)

#else

#define DIRECTERROR(hr) hr

#endif

namespace HSLL
{
	/*

	D3d11Render::state

	32λ:1��ʾʹ�õ���������Ⱦģʽ,0��ʾ����λ���ֱ��

	*/

	//��ȡ�����ַ���(��������)
	void GetErrorString(HRESULT hResult, LPCSTR File, DWORD Line);

	class D3d11Render
	{
	private:

		int Index;//״̬
		HWND hWnd;//���ھ��
		DWORD Width;//��
		DWORD Height;//��
		std::vector<DrawParam> DrawParams;//���Ʋ���

		void CreateSwapChain();//����������
		void CreateSampler();//����������
		void CreateBlendState();//�������״̬
		void CreateConstantBuffers();//������������
		void CreateRenderTarget();//����Ŀ����Ⱦ��ͼ
		void CreateShaders();//������ɫ��
		void CreateRasterizerState();//���ù�դ��
		void RecreateBuffer();//���´���һϵ�в�������Ӧ���ڴ�С
		BOOL HasDedicatedGPU();//�Ƿ�ӵ�ж����Կ��豸

	public:

		ComPtr<ID3D11Buffer> FactorBuffers_T;
		ComPtr<ID3D11Device> pDevice;//�豸
		ComPtr<IDXGISwapChain> pSwap;	//������
		ComPtr<ID3D11DeviceContext> pContext;	//�豸������	
		ComPtr<ID3D11RenderTargetView> pRenderTargetView;	//��̨����Ŀ����ͼ

		D3d11Render(HWND hWnd, DWORD Width, DWORD Height);//����

		void Present();//����

		void ClearView();//������Ⱦ��ͼ

		void SetFactors_T(float eFactor, float tFactor);//��������

		void DrawTask(TaskBase* pTask);//��������

		void ReSize(DWORD Width, DWORD Height);//���´��ڴ�С

		void SetViewPort(POINT Pos, DWORD Width, DWORD Height);//�����ӿڴ�С

		void CreateTexture(ImgBGRA* pImage, TaskTexture* pTask);//��������

		void CreateVertexBuffer(TaskBase* pTask, DWORD Width, DWORD Height);//�������㻺��
	};
}

#endif // !HS_D3D11DEVICE
