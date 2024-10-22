#include "../inc/Directx.h"

void HSLL::GetErrorString(HRESULT hResult, LPCSTR File, DWORD Line)//�����ַ���
{
	if (SUCCEEDED(hResult))
		return;

	LPSTR msgBuffer = nullptr;
	DWORD msgLength = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hResult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&msgBuffer,
		0,
		nullptr
	);

	CHAR Message[512]{};
	if (msgLength == 0)
		lstrcatA(Message, "Failed to retrieve error message.\n");
	else
		lstrcatA(Message, msgBuffer ? msgBuffer : "An unknown error occurred.");

	lstrcatA(Message, "\nFile: ");
	lstrcatA(Message, File);
	lstrcatA(Message, "\nLine: ");
	CHAR LineStr[6]{};
	_itoa_s(Line, LineStr, 10);
	lstrcatA(Message, LineStr);

	MessageBoxA(nullptr, Message, "Error", MB_OK);
	if (msgBuffer != nullptr)
		LocalFree(msgBuffer);

	throw "result error";
}


////////////////////////////////////////////////////////////////////////////////////////D3d11Render
HSLL::D3d11Render::D3d11Render(HWND hWnd, DWORD Width, DWORD Height) :hWnd(hWnd), Width(Width), Height(Height), Index(-1),DrawParams(5)
{
	this->hWnd = hWnd;
	this->Width = Width;
	this->Height = Height;
	CreateSwapChain(); // ����������
	CreateRasterizerState(); // ������դ��״̬
	CreateRenderTarget(); // ����Ŀ����Ⱦ��ͼ
	CreateShaders(); // ������ɫ��
	CreateConstantBuffers(); // ������������
	CreateSampler(); // ����������
	CreateBlendState(); // �������״̬
}


void HSLL::D3d11Render::CreateSwapChain()//����������
{
	//�������������ṹ��
	DXGI_SWAP_CHAIN_DESC swcDesc{};
	swcDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ͨ�����ز���
	swcDesc.SampleDesc.Count = 1;//ȡ��
	swcDesc.BufferDesc.Width = Width; // ���ڿ��
	swcDesc.BufferDesc.Height = Height; // ���ڸ߶�
	swcDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swcDesc.BufferCount = 2;//��Ҫ�Ļ�����Ŀ
	swcDesc.OutputWindow = hWnd;
	swcDesc.Windowed = TRUE;//�Ƿ񴰿ڻ�
	swcDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	UINT  Device_Create_Flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)

	Device_Create_Flag |= D3D11_CREATE_DEVICE_DEBUG;

#endif

	//���Լ���
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1
	};

	DWORD numFeatureLevels = sizeof(FeatureLevels) / sizeof(FeatureLevels[0]);
	D3D_DRIVER_TYPE DriverType = HasDedicatedGPU() ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_WARP;

	D3D11CreateDeviceAndSwapChain(
		nullptr,//ָ��Ĭ����������ѡ�����ȼ���ߵ��Կ����ʼǱ��Ͽ��ܽ��Ժ���Ϊ���ȣ�
		DriverType,//ѡ����������
		nullptr,//ָ����ģ������������������ʹ��GPU���㣬ѡ��ָ��������㷨��ɻ��ƣ�ʹ��CPU���㣬�ٶ�����
		Device_Create_Flag,//Ĭ�ϴ�����Ϊ��ѡ�񴴽��ı�ǩ��������DEBUGģʽ�����������Թ��ܣ�
		FeatureLevels, // �������Լ�������
		numFeatureLevels, // ���������С
		D3D11_SDK_VERSION,//��ǰʹ�õ�sdk�汾
		&swcDesc,//�����Ľ���������������ͬ����һ����
		&pSwap,
		&pDevice,
		nullptr,//���ڷ���ʵ��ʹ�õ����Լ����ָ�루������)
		&pContext
	);
}


BOOL HSLL::D3d11Render::HasDedicatedGPU()//�Ƿ�ӵ�ж����Կ��豸
{
	ComPtr<IDXGIFactory> pFactory = nullptr;
	ComPtr <IDXGIAdapter> pAdapter = nullptr;

	CreateDXGIFactory(__uuidof(IDXGIFactory), &pFactory);

	for (int i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		// NVIDIA��VendorId��0x10DE,AMD��0x1002
		if (desc.VendorId == 0x10DE || desc.VendorId == 0x1002)
			return true;// �ж����Կ�
	}

	return false;// �޶����Կ�
}


void HSLL::D3d11Render::SetFactors_T(float eFactor,float tFactor)//��������
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pContext->Map(FactorBuffers_T.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	FactorTexture* pcb = (FactorTexture*)(mappedResource.pData);
	pcb->eFactor = eFactor;
	pcb->tFactor = tFactor;
	pContext->Unmap(FactorBuffers_T.Get(), 0);
	pContext->PSSetConstantBuffers(0,1, FactorBuffers_T.GetAddressOf());
}


void HSLL::D3d11Render::ReSize(DWORD Width, DWORD Height)//���´��ڴ�С
{
	if (this->Width == Width && this->Height == Height)
		return;
	this->Width = Width;
	this->Height = Height;
	RecreateBuffer();
}


void HSLL::D3d11Render::SetViewPort(POINT Pos,DWORD Width,DWORD Height)//���´��ڴ�С
{
	// �����ӿ�
	D3D11_VIEWPORT VP;
	VP.MaxDepth = 1;
	VP.TopLeftX = Pos.x;
	VP.TopLeftY = Pos.y;
	VP.Width = Width;
	VP.Height = Height;
	VP.MinDepth = 0;
	pContext->RSSetViewports(1, &VP);

	D3D11_RECT scissorRect;
	scissorRect.left = Pos.x;
	scissorRect.top = Pos.y;
	scissorRect.right = Pos.x+Width;
	scissorRect.bottom =Pos.y+Height;
	pContext->RSSetScissorRects(1, &scissorRect);
}


void HSLL::D3d11Render::ClearView()//������Ⱦ��ͼ
{
	float FillColor[4] = { 0,0,0,0 };
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), FillColor);
}


void HSLL::D3d11Render::Present()//����
{
	pSwap->Present(1, 0);
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
}


void HSLL::D3d11Render::CreateRenderTarget()//����Ŀ����Ⱦ��ͼ
{
	//������Ⱦ��ͼ
	ComPtr<ID3D11Resource> pRenderTargetBuffer;
	DIRECTERROR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pRenderTargetBuffer));
	DIRECTERROR(pDevice->CreateRenderTargetView(pRenderTargetBuffer.Get(), nullptr, &pRenderTargetView));
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
	SetViewPort({}, Width, Height);
}


void HSLL::D3d11Render::CreateShaders()
{
	HMODULE hModule = GetModuleHandleW(NULL);

	// ���ز�����������ɫ��
	HRSRC hResP = FindResourceW(hModule, MAKEINTRESOURCE(IDR_HLSL1), L"HLSL");
	HGLOBAL hMemP = LoadResource(hModule, hResP);
	void* pPixelShaderData = LockResource(hMemP);
	DWORD pixelShaderSize = SizeofResource(hModule, hResP);
	DIRECTERROR(pDevice->CreatePixelShader(pPixelShaderData, pixelShaderSize, nullptr, &DrawParams[0].pPixelShader));

	// ���ز�����������ɫ��
	HRSRC hResV = FindResourceW(hModule, MAKEINTRESOURCE(IDR_HLSL2), L"HLSL");
	HGLOBAL hMemV = LoadResource(hModule, hResV);
	void* pVertexShaderData = LockResource(hMemV);
	DWORD vertexShaderSize = SizeofResource(hModule, hResV);
	DIRECTERROR(pDevice->CreateVertexShader(pVertexShaderData, vertexShaderSize, nullptr, &DrawParams[0].pVertexShader));

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
	  {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	  {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	DIRECTERROR(pDevice->CreateInputLayout(inputDesc, 2, pVertexShaderData, vertexShaderSize, &DrawParams[0].pInputLayout));
 
	DrawParams[0].Topology= D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
}


void HSLL::D3d11Render::CreateRasterizerState()//���ù�դ��
{
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.CullMode = D3D11_CULL_NONE;//���ñ����޳�
	rasterDesc.ScissorEnable = true; // ʹ�ܲü�
	rasterDesc.FillMode = D3D11_FILL_SOLID; // ʹ��ʵ�Ķ�������ģʽ


	// ������դ����״̬����
	ComPtr<ID3D11RasterizerState> pSolidRasterState;
	DIRECTERROR(pDevice->CreateRasterizerState(&rasterDesc, &pSolidRasterState));
	pContext->RSSetState(pSolidRasterState.Get());
}


void HSLL::D3d11Render::CreateSampler()//����������
{
	//����������,���������������������ȡ����
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;//���ù�����
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	ComPtr<ID3D11SamplerState> pSampler;
	DIRECTERROR(pDevice->CreateSamplerState(&samplerDesc, &pSampler));
	pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());
}


void HSLL::D3d11Render::CreateBlendState()//�������״̬
{
	//�������
	D3D11_BLEND_DESC blendDesc{};

	// Ϊ��ȾĿ��0���û��
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//���
	ComPtr<ID3D11BlendState>pBst;
	DIRECTERROR(pDevice->CreateBlendState(&blendDesc, pBst.GetAddressOf()));
	pContext->OMSetBlendState(pBst.Get(), nullptr, 0xFFFFFFFF);
}


void HSLL::D3d11Render::CreateConstantBuffers()//������������
{
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(FactorTexture);
	DIRECTERROR(pDevice->CreateBuffer(&cbDesc, nullptr, &FactorBuffers_T));
}


void HSLL::D3d11Render::CreateTexture(ImgBGRA* pImage, TaskTexture* pTask)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ��ǰ����255��Χ����ɫ����ת��Ϊ������
	tDesc.ArraySize = 1;
	tDesc.MipLevels = 1;
	tDesc.SampleDesc = { 1, 0 };
	tDesc.Width = pImage->Width;
	tDesc.Height = pImage->Height;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA tData = { pImage->Data, pImage->Width * 4, 0 };
	DIRECTERROR(pDevice->CreateTexture2D(&tDesc, &tData, &pTask->pTexture2D));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(pTask->pTexture2D.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, tDesc.Format);
	DIRECTERROR(pDevice->CreateShaderResourceView(pTask->pTexture2D.Get(), &srvDesc, &pTask->pSourse));
}


void HSLL::D3d11Render::CreateVertexBuffer(TaskBase* pTask, DWORD Width, DWORD Height)//�������㻺��
{
	static BYTE StaticBuffer[4096];
	void* pSysMem;

	//������Դ����
	D3D11_BUFFER_DESC bDesc{};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//���㻺��
	bDesc.StructureByteStride = pTask->VertexPersize;//���벽��������Ԫ�ش�С��
	bDesc.ByteWidth = bDesc.StructureByteStride * pTask->VerticesNum;//�ܹ���С

	D3D11_SUBRESOURCE_DATA vSubData{};	//������Դ����

	if (bDesc.ByteWidth > 4096)
		pSysMem = malloc(bDesc.ByteWidth);
	else
		pSysMem = StaticBuffer;

	vSubData.pSysMem = pSysMem;
	pTask->FillVerticeBuffer(Width, Height, pSysMem);
	DIRECTERROR(pDevice->CreateBuffer(&bDesc, &vSubData, &pTask->pVertexBuffer));	//����������Դ������

	if (bDesc.ByteWidth > 4096)
		free(pSysMem);
}


void HSLL::D3d11Render::DrawTask(TaskBase* pTask)//��������
{
	if (Index != pTask->ParamIndex)
	{
		pContext->IASetPrimitiveTopology(DrawParams[pTask->ParamIndex].Topology);
		pContext->PSSetShader(DrawParams[pTask->ParamIndex].pPixelShader.Get(), nullptr, 0);
		pContext->VSSetShader(DrawParams[pTask->ParamIndex].pVertexShader.Get(), nullptr, 0);
		pContext->IASetInputLayout(DrawParams[pTask->ParamIndex].pInputLayout.Get());
	}
	
	pTask->FuncAdditon(this);

	UINT Persize = pTask->VertexPersize;//�������ݴ�С
	UINT Offset = 0;//��Чƫ��
	pContext->IASetVertexBuffers(0, 1, pTask->pVertexBuffer.GetAddressOf(), &Persize, &Offset);
	pContext->Draw(pTask->VerticesNum, 0);
	Index = pTask->ParamIndex;
}


void HSLL::D3d11Render::RecreateBuffer()//���´�����������Ӧ���ڴ�С
{
	pRenderTargetView.Reset();

	pSwap->ResizeBuffers(2, Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	CreateRenderTarget();
}
