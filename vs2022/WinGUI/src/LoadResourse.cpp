#include "../inc/LoadResourse.h"


const HSLL::ColorRGBA HSLL::ColorRGBA::TransParent = { 0,0,0,0 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Black = { 0,0,0,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Red = { 255,0,0,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Green = { 0,255,0,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Blue = { 0,0,255,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::White = { 255,255,255,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Yellow = { 255,255,0,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Purple = { 255,0,255,255 };
const HSLL::ColorRGBA HSLL::ColorRGBA::Cyan = { 0,255,255,255 };


HSLL::ImgBGRA* HSLL::LoadPng(LPCSTR FilePath)//载入图片
{
	if (GetFileAttributesA(FilePath) == INVALID_FILE_ATTRIBUTES)
		return nullptr;

	//读取文件信息
	FILE* fp = fopen(FilePath, "rb");
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)abort();
	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	//文件指针指向数据部分
	png_init_io(png, fp);
	png_read_info(png, info);

	ImgBGRA* rStruct = new ImgBGRA;
	rStruct->Width = png_get_image_width(png, info);
	rStruct->Height = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth = png_get_bit_depth(png, info);

	//格式化数据为b8g8r8a8数据
	if (bit_depth == 16)
		png_set_strip_16(png);
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);
	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	// 分配内存以存储整个图像
	DWORD Stride = rStruct->Width * 4;
	rStruct->Data = new BYTE[Stride * rStruct->Height];

	// 直接读取每一行数据
	for (int y = 0; y < rStruct->Height; y++) 
	{
		png_bytep rowPtr = rStruct->Data + (y * Stride);
		png_read_rows(png, &rowPtr, nullptr, 1);
	}

	png_destroy_read_struct(&png, &info, NULL);

	return rStruct;
}


struct MemoryReader {
	const unsigned char* data;
	size_t size;
	size_t offset;
};


void ReadFromMemory(png_structp pngPtr, png_bytep outBytes, png_size_t byteCountToRead) {
	MemoryReader* reader = (MemoryReader*)png_get_io_ptr(pngPtr);
	if (reader->offset + byteCountToRead <= reader->size) {
		memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
		reader->offset += byteCountToRead;
	}
	else {
		png_error(pngPtr, "ReadFromMemory failed");
	}
}


HSLL::ImgBGRA* HSLL::LoadPng(BYTE* Data, DWORD Size)
{
	if (Data == nullptr || Size == 0)
		return nullptr;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();
	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	MemoryReader reader = { Data, Size, 0 };
	png_set_read_fn(png, &reader, ReadFromMemory);

	png_read_info(png, info);

	ImgBGRA* rStruct = new ImgBGRA;
	rStruct->Width = png_get_image_width(png, info);
	rStruct->Height = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth = png_get_bit_depth(png, info);

	if (bit_depth == 16)
		png_set_strip_16(png);
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);
	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	DWORD Stride = rStruct->Width * 4;
	rStruct->Data = new BYTE[Stride * rStruct->Height];

	// 直接读取到数据缓冲区中
	for (int y = 0; y < rStruct->Height; y++) {
		png_bytep rowPtr = rStruct->Data + (y * Stride);
		png_read_rows(png, &rowPtr, nullptr, 1);
	}

	png_destroy_read_struct(&png, &info, NULL);

	return rStruct;
}



HSLL::ImgBGRA* HSLL::LoadBmp(LPCSTR FilePath)//加载bmp图片，创建数据对象
{
	HANDLE hfile = CreateFileA(FilePath, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
		return nullptr;

	struct BITMAPHEADER
	{
		BITMAPFILEHEADER header;
		tagBITMAPINFOHEADER infoheader;
	};

	BITMAPHEADER Header{};
	ReadFile(hfile, &Header.header, 14, nullptr, nullptr);
	ReadFile(hfile, &Header.infoheader, 40, nullptr, nullptr);

	if (Header.infoheader.biBitCount == 24)
		return nullptr;

	ImgBGRA* rStruct = nullptr;

	if (Header.infoheader.biBitCount == 32 || Header.infoheader.biBitCount == 24)
	{
		rStruct = new ImgBGRA();
		rStruct->Data = new BYTE[Header.infoheader.biSizeImage];
		rStruct->Height = Header.infoheader.biHeight;
		rStruct->Width = Header.infoheader.biWidth;
		DWORD Stride = rStruct->Width * Header.infoheader.biBitCount / 8;
		BYTE* Ptr = rStruct->Data + Stride * (rStruct->Height - 1);

		for (int i = 0; i < rStruct->Height; i++)
		{
			ReadFile(hfile, rStruct->Data, Stride, nullptr, nullptr);
			Ptr -= Stride;
		}
	}
	CloseHandle(hfile);
	return rStruct;
}


HSLL::ImgBGRA* HSLL::LoadBmp(BYTE* Buffer, DWORD Size)
{
	if (!Buffer || Size < sizeof(BITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER))
		return nullptr;

	const BYTE* pCurr = Buffer;

	// Read bitmap headers
	BITMAPFILEHEADER header;
	tagBITMAPINFOHEADER infoheader;

	memcpy(&header, pCurr, sizeof(BITMAPFILEHEADER));
	pCurr += sizeof(BITMAPFILEHEADER);
	memcpy(&infoheader, pCurr, sizeof(tagBITMAPINFOHEADER));
	pCurr += sizeof(tagBITMAPINFOHEADER);

	if (infoheader.biBitCount == 24)
		return nullptr;

	ImgBGRA* rStruct = nullptr;

	if (infoheader.biBitCount == 32 || infoheader.biBitCount == 24)
	{
		rStruct = new ImgBGRA();
		rStruct->Data = new BYTE[infoheader.biSizeImage];
		rStruct->Height = infoheader.biHeight;
		rStruct->Width = infoheader.biWidth;
		DWORD Stride = rStruct->Width * infoheader.biBitCount / 8;
		BYTE* Ptr = rStruct->Data + Stride * (rStruct->Height - 1);

		for (int i = 0; i < rStruct->Height; i++)
		{
			memcpy(Ptr, pCurr, Stride);
			pCurr += Stride;
			Ptr -= Stride;
		}
	}

	return rStruct;
}



HICON CreateIconFromRGBA(BYTE* rgbaData, DWORD width, DWORD height)
{
	HBITMAP colorBitmap = NULL;
	HBITMAP maskBitmap = NULL;
	HICON hIcon = NULL;

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -1*height; 
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hdc = GetDC(NULL);
	BYTE* pBits = NULL;
	colorBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	ReleaseDC(NULL, hdc);

	if (colorBitmap == NULL)
		return NULL;

	for (int i = 0; i < width * height; ++i)
	{
		pBits[i * 4 + 0] = rgbaData[i * 4 + 2]; 
		pBits[i * 4 + 1] = rgbaData[i * 4 + 1]; 
		pBits[i * 4 + 2] = rgbaData[i * 4 + 0]; 
		pBits[i * 4 + 3] = rgbaData[i * 4 + 3]; 
	}

	maskBitmap = CreateBitmap(width, height, 1, 1, NULL);
	if (maskBitmap == NULL)
	{
		DeleteObject(colorBitmap);
		return NULL;
	}

	ICONINFO iconInfo = { 0 };
	iconInfo.fIcon = TRUE;
	iconInfo.hbmColor = colorBitmap;
	iconInfo.hbmMask = maskBitmap;

	hIcon = CreateIconIndirect(&iconInfo);

	DeleteObject(colorBitmap);
	DeleteObject(maskBitmap);

	return hIcon;
}


HCURSOR CreateCursorFromRGBA(BYTE* rgbaData, DWORD width, DWORD height, DWORD xHotspot, DWORD yHotspot)
{
	HBITMAP colorBitmap = NULL;
	HBITMAP maskBitmap = NULL;
	HCURSOR hCursor = NULL;

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -1*height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	BYTE* bitmapData = NULL;
	HDC hdc = GetDC(NULL);
	colorBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&bitmapData, NULL, 0);
	ReleaseDC(NULL, hdc);

	if (colorBitmap == NULL)
		return NULL;

	for (int i = 0; i < width * height * 4; i += 4)
	{
		bitmapData[i] = rgbaData[i + 2]; 
		bitmapData[i + 1] = rgbaData[i + 1]; 
		bitmapData[i + 2] = rgbaData[i];     
		bitmapData[i + 3] = rgbaData[i + 3]; 
	}

	maskBitmap = CreateBitmap(width, height, 1, 1, NULL);
	if (maskBitmap == NULL)
	{
		DeleteObject(colorBitmap);
		return NULL;
	}

	ICONINFO iconInfo = { 0 };
	iconInfo.fIcon = FALSE; 
	iconInfo.xHotspot = xHotspot;
	iconInfo.yHotspot = yHotspot;
	iconInfo.hbmColor = colorBitmap;
	iconInfo.hbmMask = maskBitmap;

	hCursor = CreateIconIndirect(&iconInfo);

	DeleteObject(colorBitmap);
	DeleteObject(maskBitmap);

	return hCursor;
}

HICON HSLL::CreateIcon(LPCSTR PathPng)
{
	if (!PathPng)
		return 0;

	ImgBGRA* pData = LoadPng(PathPng);
	HICON hIcon = CreateIconFromRGBA(pData->Data, pData->Width, pData->Height);
	delete pData;
	return hIcon;
}


HICON HSLL::CreateIcon(BYTE* rgbaData, DWORD  width, DWORD height)
{
	if (!rgbaData)
		return 0;

	return CreateIconFromRGBA(rgbaData, width, height);
}


HCURSOR HSLL::CreateCursor(LPCSTR PathPng, DWORD  xHotspot, DWORD yHotspot)
{
	if (!PathPng)
		return 0;

	ImgBGRA* pData = LoadPng(PathPng);
	HICON hIcon = CreateCursorFromRGBA(pData->Data, pData->Width, pData->Height, xHotspot, yHotspot);
	delete pData;
	return hIcon;
}


HCURSOR HSLL::CreateCursor(BYTE* rgbaData, DWORD  width, DWORD  height, DWORD  xHotspot, DWORD yHotspot)
{
	if (!rgbaData)
		return 0;

	if (!rgbaData)
		return 0;

	return  CreateCursorFromRGBA(rgbaData, width, height, xHotspot, yHotspot);
}


HSLL::ImgBGRA::~ImgBGRA()//析构
{
	if (Data)
		delete[] Data;
}