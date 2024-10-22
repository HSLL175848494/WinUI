#include "../inc/Directx.h"

void HSLL::GetErrorString(HRESULT hResult, LPCSTR File, DWORD Line)//错误字符串
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
	CreateSwapChain(); // 创建交换链
	CreateRasterizerState(); // 创建光栅器状态
	CreateRenderTarget(); // 创建目标渲染视图
	CreateShaders(); // 创建着色器
	CreateConstantBuffers(); // 创建常量缓冲
	CreateSampler(); // 创建采样器
	CreateBlendState(); // 创建混合状态
}


void HSLL::D3d11Render::CreateSwapChain()//创建交换链
{
	//交换链的描述结构体
	DXGI_SWAP_CHAIN_DESC swcDesc{};
	swcDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//通道像素布局
	swcDesc.SampleDesc.Count = 1;//取样
	swcDesc.BufferDesc.Width = Width; // 窗口宽度
	swcDesc.BufferDesc.Height = Height; // 窗口高度
	swcDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swcDesc.BufferCount = 2;//需要的缓存数目
	swcDesc.OutputWindow = hWnd;
	swcDesc.Windowed = TRUE;//是否窗口化
	swcDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	UINT  Device_Create_Flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)

	Device_Create_Flag |= D3D11_CREATE_DEVICE_DEBUG;

#endif

	//特性级别
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
		nullptr,//指定默认适配器（选择优先级最高的显卡。笔记本上可能将显核作为优先）
		DriverType,//选择驱动类型
		nullptr,//指定的模拟软件句柄（填入后代表不使用GPU运算，选用指定的软件算法完成绘制，使用CPU计算，速度慢）
		Device_Create_Flag,//默认创建行为（选择创建的标签，例如以DEBUG模式创建附带调试功能）
		FeatureLevels, // 传递特性级别数组
		numFeatureLevels, // 传递数组大小
		D3D11_SDK_VERSION,//当前使用的sdk版本
		&swcDesc,//创建的交换链的描述（如同设置一样）
		&pSwap,
		&pDevice,
		nullptr,//用于返回实际使用的特性级别的指针（不接收)
		&pContext
	);
}


BOOL HSLL::D3d11Render::HasDedicatedGPU()//是否拥有独立显卡设备
{
	ComPtr<IDXGIFactory> pFactory = nullptr;
	ComPtr <IDXGIAdapter> pAdapter = nullptr;

	CreateDXGIFactory(__uuidof(IDXGIFactory), &pFactory);

	for (int i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		// NVIDIA的VendorId是0x10DE,AMD是0x1002
		if (desc.VendorId == 0x10DE || desc.VendorId == 0x1002)
			return true;// 有独立显卡
	}

	return false;// 无独立显卡
}


void HSLL::D3d11Render::SetFactors_T(float eFactor,float tFactor)//设置因子
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pContext->Map(FactorBuffers_T.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	FactorTexture* pcb = (FactorTexture*)(mappedResource.pData);
	pcb->eFactor = eFactor;
	pcb->tFactor = tFactor;
	pContext->Unmap(FactorBuffers_T.Get(), 0);
	pContext->PSSetConstantBuffers(0,1, FactorBuffers_T.GetAddressOf());
}


void HSLL::D3d11Render::ReSize(DWORD Width, DWORD Height)//更新窗口大小
{
	if (this->Width == Width && this->Height == Height)
		return;
	this->Width = Width;
	this->Height = Height;
	RecreateBuffer();
}


void HSLL::D3d11Render::SetViewPort(POINT Pos,DWORD Width,DWORD Height)//更新窗口大小
{
	// 设置视口
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


void HSLL::D3d11Render::ClearView()//清理渲染视图
{
	float FillColor[4] = { 0,0,0,0 };
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), FillColor);
}


void HSLL::D3d11Render::Present()//呈现
{
	pSwap->Present(1, 0);
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
}


void HSLL::D3d11Render::CreateRenderTarget()//创建目标渲染视图
{
	//设置渲染视图
	ComPtr<ID3D11Resource> pRenderTargetBuffer;
	DIRECTERROR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pRenderTargetBuffer));
	DIRECTERROR(pDevice->CreateRenderTargetView(pRenderTargetBuffer.Get(), nullptr, &pRenderTargetView));
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
	SetViewPort({}, Width, Height);
}


void HSLL::D3d11Render::CreateShaders()
{
	HMODULE hModule = GetModuleHandleW(NULL);

	// 加载并创建像素着色器
	HRSRC hResP = FindResourceW(hModule, MAKEINTRESOURCE(IDR_HLSL1), L"HLSL");
	HGLOBAL hMemP = LoadResource(hModule, hResP);
	void* pPixelShaderData = LockResource(hMemP);
	DWORD pixelShaderSize = SizeofResource(hModule, hResP);
	DIRECTERROR(pDevice->CreatePixelShader(pPixelShaderData, pixelShaderSize, nullptr, &DrawParams[0].pPixelShader));

	// 加载并创建顶点着色器
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


void HSLL::D3d11Render::CreateRasterizerState()//设置光栅器
{
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.CullMode = D3D11_CULL_NONE;//禁用背面剔除
	rasterDesc.ScissorEnable = true; // 使能裁剪
	rasterDesc.FillMode = D3D11_FILL_SOLID; // 使用实心多边形填充模式


	// 创建光栅化器状态对象
	ComPtr<ID3D11RasterizerState> pSolidRasterState;
	DIRECTERROR(pDevice->CreateRasterizerState(&rasterDesc, &pSolidRasterState));
	pContext->RSSetState(pSolidRasterState.Get());
}


void HSLL::D3d11Render::CreateSampler()//创建采样器
{
	//创建采样器,根据纹理坐标从纹理中提取像素
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;//设置过滤器
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	ComPtr<ID3D11SamplerState> pSampler;
	DIRECTERROR(pDevice->CreateSamplerState(&samplerDesc, &pSampler));
	pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());
}


void HSLL::D3d11Render::CreateBlendState()//创建混合状态
{
	//混合描述
	D3D11_BLEND_DESC blendDesc{};

	// 为渲染目标0启用混合
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//混合
	ComPtr<ID3D11BlendState>pBst;
	DIRECTERROR(pDevice->CreateBlendState(&blendDesc, pBst.GetAddressOf()));
	pContext->OMSetBlendState(pBst.Get(), nullptr, 0xFFFFFFFF);
}


void HSLL::D3d11Render::CreateConstantBuffers()//创建常量缓冲
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
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 当前设置255范围的颜色数据转化为浮点数
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


void HSLL::D3d11Render::CreateVertexBuffer(TaskBase* pTask, DWORD Width, DWORD Height)//创建顶点缓冲
{
	static BYTE StaticBuffer[4096];
	void* pSysMem;

	//创建资源描述
	D3D11_BUFFER_DESC bDesc{};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//顶点缓冲
	bDesc.StructureByteStride = pTask->VertexPersize;//填入步幅（单个元素大小）
	bDesc.ByteWidth = bDesc.StructureByteStride * pTask->VerticesNum;//总共大小

	D3D11_SUBRESOURCE_DATA vSubData{};	//填入资源数据

	if (bDesc.ByteWidth > 4096)
		pSysMem = malloc(bDesc.ByteWidth);
	else
		pSysMem = StaticBuffer;

	vSubData.pSysMem = pSysMem;
	pTask->FillVerticeBuffer(Width, Height, pSysMem);
	DIRECTERROR(pDevice->CreateBuffer(&bDesc, &vSubData, &pTask->pVertexBuffer));	//创建顶点资源缓冲区

	if (bDesc.ByteWidth > 4096)
		free(pSysMem);
}


void HSLL::D3d11Render::DrawTask(TaskBase* pTask)//绘制任务
{
	if (Index != pTask->ParamIndex)
	{
		pContext->IASetPrimitiveTopology(DrawParams[pTask->ParamIndex].Topology);
		pContext->PSSetShader(DrawParams[pTask->ParamIndex].pPixelShader.Get(), nullptr, 0);
		pContext->VSSetShader(DrawParams[pTask->ParamIndex].pVertexShader.Get(), nullptr, 0);
		pContext->IASetInputLayout(DrawParams[pTask->ParamIndex].pInputLayout.Get());
	}
	
	pTask->FuncAdditon(this);

	UINT Persize = pTask->VertexPersize;//单个数据大小
	UINT Offset = 0;//有效偏移
	pContext->IASetVertexBuffers(0, 1, pTask->pVertexBuffer.GetAddressOf(), &Persize, &Offset);
	pContext->Draw(pTask->VerticesNum, 0);
	Index = pTask->ParamIndex;
}


void HSLL::D3d11Render::RecreateBuffer()//重新创建参数以适应窗口大小
{
	pRenderTargetView.Reset();

	pSwap->ResizeBuffers(2, Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	CreateRenderTarget();
}
