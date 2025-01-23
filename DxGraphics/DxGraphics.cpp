#include "DxGraphics.h"
#include <wincodec.h>
#include <wincodecsdk.h>
#include <wtypes.h>
#include <windowsx.h>

// 链接 D2D1 和 DWrite 库
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// DxImagingFactory 类的构造函数
DxImagingFactory::DxImagingFactory()
{
	pImagingFactory = nullptr; // 初始化指针为 nullptr
}

// DxImagingFactory 类的析构函数
DxImagingFactory::~DxImagingFactory()
{
	Uninitialize(); // 调用 Uninitialize 方法释放资源
}

// 初始化 WIC (Windows Imaging Component) 工厂
HRESULT DxImagingFactory::Initialize()
{
	if (pImagingFactory) return S_OK; // 如果已初始化，则直接返回 S_OK
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, // WIC 工厂的类 ID
		NULL,                    // 不使用聚合
		CLSCTX_INPROC_SERVER,    // 在 DLL 中运行代码
		IID_PPV_ARGS(&pImagingFactory) // 接口 ID 和指向接收接口指针的地址
	);
	return hr; // 返回初始化结果
}

// 释放 WIC 工厂资源
void DxImagingFactory::Uninitialize()
{
	if (pImagingFactory) pImagingFactory->Release(); // 释放接口
	pImagingFactory = nullptr; // 将指针置为 nullptr
}

// 返回 WIC 工厂指针
DxImagingFactory::ImagingFactory* DxImagingFactory::Factory()
{
	return pImagingFactory;
}

// 类型转换运算符，返回 WIC 工厂指针
DxImagingFactory::operator ImagingFactory* ()
{
	return pImagingFactory;
}

// DxWriteFactory 类的构造函数
DxWriteFactory::DxWriteFactory()
{
	pWriteFactory = nullptr; // 初始化指针为 nullptr
}

// DxWriteFactory 类的析构函数
DxWriteFactory::~DxWriteFactory()
{
	Uninitialize(); // 调用 Uninitialize 方法释放资源
}

// 初始化 DirectWrite 工厂
HRESULT DxWriteFactory::Initialize()
{
	if (pWriteFactory) return E_FAIL; // 如果已初始化，则返回 E_FAIL
	return DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, // 共享工厂
		__uuidof(IDWriteFactory),   // DirectWrite 工厂的接口 ID
		reinterpret_cast<IUnknown**>(&pWriteFactory) // 接收接口指针的地址
	);
}

// 释放 DirectWrite 工厂资源
void DxWriteFactory::Uninitialize()
{
	if (pWriteFactory) pWriteFactory->Release(); // 释放接口
	pWriteFactory = nullptr; // 将指针置为 nullptr
}

// 返回 DirectWrite 工厂指针
DxWriteFactory::WriteFactory* DxWriteFactory::Factory()
{
	return pWriteFactory;
}

// 类型转换运算符，返回 DirectWrite 工厂指针
DxWriteFactory::operator WriteFactory* ()
{
	return pWriteFactory;
}

// DxFontCollection 类的构造函数
DxFontCollection::DxFontCollection()
{
	pMemoryLoader = nullptr;   // 初始化指针为 nullptr
	pFontCollection = nullptr;
	pWriteFactory = nullptr;
	lpszFontFamily = nullptr;
}

// DxFontCollection 类的析构函数
DxFontCollection::~DxFontCollection()
{
	Uninitialize(); // 调用 Uninitialize 方法释放资源
}

// 从文件初始化字体集合
HRESULT DxFontCollection::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath)
{
	if (pFontCollection) return E_FAIL; // 如果已初始化，则返回 E_FAIL
	IDWriteFontSetBuilder1* pFSBuilder = nullptr;
	IDWriteFontSet* pFontSet = nullptr;
	IDWriteFontFile* pFontFile = nullptr;
	IDWriteFontCollection1* pFCollection = nullptr;
	IDWriteFontFamily* pFFamily = nullptr;
	IDWriteLocalizedStrings* pFFName = nullptr;

	// 创建字体集构建器
	HRESULT hr = pWriteFactory->CreateFontSetBuilder(&pFSBuilder);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontFileReference(szPath, /* lastWriteTime*/ nullptr, &pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->AddFontFile(pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->CreateFontSet(&pFontSet);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &pFCollection);
	if (SUCCEEDED(hr)) hr = pFCollection->GetFontFamily(0, &pFFamily);
	if (SUCCEEDED(hr)) hr = pFFamily->GetFamilyNames(&pFFName);

	UINT32 len = 0;
	if (SUCCEEDED(hr)) hr = pFFName->GetStringLength(0, &len);
	if (SUCCEEDED(hr)) lpszFontFamily = new WCHAR[len + 1];
	if (SUCCEEDED(hr)) hr = pFFName->GetString(0, lpszFontFamily, len + 1);
	if (SUCCEEDED(hr)) lpszFontFamily[len] = 0;

	// 释放资源
	if (pFSBuilder) pFSBuilder->Release();
	if (pFontSet) pFontSet->Release();
	if (pFontFile) pFontFile->Release();
	if (pFFamily) pFFamily->Release();
	if (pFFName) pFFName->Release();
	if (!SUCCEEDED(hr) && pFCollection) pFCollection->Release();

	if (SUCCEEDED(hr)) {
		this->pFontCollection = pFCollection;
		this->pWriteFactory = pWriteFactory;
	}

	if (!SUCCEEDED(hr) && lpszFontFamily) {
		delete[] lpszFontFamily;
		lpszFontFamily = nullptr;
	}
	return hr;
}

// 从资源初始化字体集合
HRESULT DxFontCollection::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType)
{
	if (pFontCollection) return E_FAIL; // 如果已初始化，则返回 E_FAIL
	IDWriteFontSetBuilder1* pFSBuilder = nullptr;
	IDWriteFontSet* pFontSet = nullptr;
	IDWriteFontFile* pFontFile = nullptr;
	IDWriteFontCollection1* pFCollection = nullptr;
	IDWriteFontFamily* pFFamily = nullptr;
	IDWriteLocalizedStrings* pFFName = nullptr;

	HRSRC hRscFont = nullptr;
	HGLOBAL hGlobalFont = nullptr;
	void* pFontData = nullptr;
	DWORD nFDSize = 0;

	// 查找资源
	hRscFont = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscFont ? S_OK : E_FAIL;

	if (SUCCEEDED(hr)) {
		// 加载资源
		hGlobalFont = LoadResource(hModule, hRscFont);
		hr = hGlobalFont ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// 锁定资源以获取内存指针
		pFontData = LockResource(hGlobalFont);
		hr = pFontData ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// 计算资源大小
		nFDSize = SizeofResource(hModule, hRscFont);
		hr = nFDSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontSetBuilder(&pFSBuilder);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateInMemoryFontFileLoader(&this->pMemoryLoader);
	if (SUCCEEDED(hr)) hr = pWriteFactory->RegisterFontFileLoader(this->pMemoryLoader);
	if (SUCCEEDED(hr)) hr = this->pMemoryLoader->CreateInMemoryFontFileReference(pWriteFactory, pFontData, nFDSize, nullptr, &pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->AddFontFile(pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->CreateFontSet(&pFontSet);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &pFCollection);
	if (SUCCEEDED(hr)) hr = pFCollection->GetFontFamily(0, &pFFamily);
	if (SUCCEEDED(hr)) hr = pFFamily->GetFamilyNames(&pFFName);

	UINT32 len = 0;
	if (SUCCEEDED(hr)) hr = pFFName->GetStringLength(0, &len);
	if (SUCCEEDED(hr)) lpszFontFamily = new WCHAR[len + 1];
	if (SUCCEEDED(hr)) hr = pFFName->GetString(0, lpszFontFamily, len + 1);
	if (SUCCEEDED(hr)) lpszFontFamily[len] = 0;

	// 释放资源
	if (pFSBuilder) pFSBuilder->Release();
	if (pFontSet) pFontSet->Release();
	if (pFontFile) pFontFile->Release();
	if (pFFamily) pFFamily->Release();
	if (pFFName) pFFName->Release();
	if (!SUCCEEDED(hr) && pFCollection) pFCollection->Release();

	if (SUCCEEDED(hr)) {
		this->pFontCollection = pFCollection;
		this->pWriteFactory = pWriteFactory;
	}

	if (!SUCCEEDED(hr) && lpszFontFamily) {
		delete[] lpszFontFamily;
		lpszFontFamily = nullptr;
	}
	return hr;
}

// 释放字体集合资源
void DxFontCollection::Uninitialize()
{
	if (pFontCollection) pFontCollection->Release(); // 释放字体集合
	if (pMemoryLoader) {
		pWriteFactory->UnregisterFontFileLoader(pMemoryLoader); // 注销字体文件加载器
		pMemoryLoader->Release(); // 释放加载器
	}
	if (lpszFontFamily) delete[] lpszFontFamily; // 释放字体家族名称
	pFontCollection = nullptr;
	pMemoryLoader = nullptr;
	pWriteFactory = nullptr;
	lpszFontFamily = nullptr;
}

// 返回字体集合指针
DxFontCollection::WriteFontCollection* DxFontCollection::FontCollection()
{
	return pFontCollection;
}

// 返回 DirectWrite 工厂指针
DxWriteFactory::WriteFactory* DxFontCollection::WriteFactory()
{
	return pWriteFactory;
}

// 返回字体家族名称
LPCWSTR DxFontCollection::FontFamily() const
{
	return lpszFontFamily;
}

// 类型转换运算符，返回字体集合指针
DxFontCollection::operator WriteFontCollection* ()
{
	return pFontCollection;
}

// 类型转换运算符，返回 DirectWrite 工厂指针
DxFontCollection::operator DxWriteFactory::WriteFactory* ()
{
	return pWriteFactory;
}

// DxTextFormat 类的构造函数
DxTextFormat::DxTextFormat()
{
	pTextFormat = nullptr; // 初始化指针为 nullptr
	pWriteFactory = nullptr;
	fFontSize = 0.0f;
}

// DxTextFormat 类的析构函数
DxTextFormat::~DxTextFormat()
{
	Uninitialize(); // 调用 Uninitialize 方法释放资源
}

// 初始化文本格式对象
HRESULT DxTextFormat::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName)
{
	this->pWriteFactory = pWriteFactory;
	this->fFontSize = fFontSize;
	return pWriteFactory->CreateTextFormat(
		szFontFamilyName ? szFontFamilyName : L"Microsoft YaHei", // 字体家族名称
		pFontCollection, // 字体集合
		DWRITE_FONT_WEIGHT_REGULAR, // 字体粗细
		DWRITE_FONT_STYLE_NORMAL, // 字体样式
		DWRITE_FONT_STRETCH_NORMAL, // 字体拉伸
		fFontSize, // 字体大小
		szLocaleName, // 区域设置名称
		&pTextFormat // 接收文本格式对象的地址
	);
}

// 使用字体集合初始化文本格式对象
HRESULT DxTextFormat::Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName)
{
	return Initialize(pFontCollection->WriteFactory(), pFontCollection->FontFamily(), pFontCollection->FontCollection(), fFontSize, szLocaleName);
}

// 释放文本格式对象资源
void DxTextFormat::Uninitialize()
{
	if (pTextFormat) pTextFormat->Release(); // 释放文本格式对象
	pTextFormat = nullptr;
	fFontSize = 0.0f;
}

// 返回文本格式对象指针
DxTextFormat::WriteTextFormat* DxTextFormat::TextFormat()
{
	return pTextFormat;
}

// 返回 DirectWrite 工厂指针
DxWriteFactory::WriteFactory* DxTextFormat::WriteFactory()
{
	return pWriteFactory;
}

// 返回字体大小
float DxTextFormat::FontSize() const
{
	return fFontSize;
}

// 类型转换运算符，返回文本格式对象指针
DxTextFormat::operator WriteTextFormat* ()
{
	return pTextFormat;
}

// DxFactory2D 类的构造函数
DxFactory2D::DxFactory2D()
{
	pFactory = nullptr; // 初始化指针为 nullptr
}

// DxFactory2D 类的析构函数
DxFactory2D::~DxFactory2D()
{
	Uninitialize(); // 调用 Uninitialize 方法释放资源
}

// 初始化 Direct2D 工厂
HRESULT DxFactory2D::Initialize()
{
	return D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pFactory);
}

// 释放 Direct2D 工厂资源
void DxFactory2D::Uninitialize()
{
	if (pFactory) pFactory->Release(); // 释放工厂对象
	pFactory = nullptr;
}

// 返回 Direct2D 工厂指针
DxFactory2D::D2DFactory* DxFactory2D::Factory()
{
	return pFactory;
}

// 类型转换运算符，返回 Direct2D 工厂指针
DxFactory2D::operator D2DFactory* ()
{
	return pFactory;
}

// DxTarget2D 类的构造函数
DxTarget2D::DxTarget2D()
{
	pTarget = nullptr; // 初始化指针为 nullptr
	hWnd = nullptr;
	interlayer = GDI_ON_DIRECT2D;
	tagDraw = false;
}

// DxTarget2D 类的析构函数
DxTarget2D::~DxTarget2D()
{
	Uninitialize(); // 调用 Uninitialize 方法释放资源
}

// 初始化 Direct2D 渲染目标
HRESULT DxTarget2D::Initialize(DxFactory2D::D2DFactory* p2DFactory, HWND hWnd)
{
	if (pTarget) return E_FAIL; // 如果已初始化，则返回 E_FAIL
	RECT Rect;
	GetClientRect(hWnd, &Rect);
	D2D1_SIZE_U size = D2D1::SizeU(Rect.right - Rect.left, Rect.bottom - Rect.top);
	HRESULT hr = p2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, size),
		&pTarget);
	if (SUCCEEDED(hr)) this->hWnd = hWnd;
	return hr;
}

// 释放 Direct2D 渲染目标资源
void DxTarget2D::Uninitialize()
{
	if (pTarget) pTarget->Release(); // 释放渲染目标对象
	pTarget = nullptr;
	hWnd = nullptr;
	interlayer = GDI_ON_DIRECT2D;
	tagDraw = false;
}

// 设置绘制模式
void DxTarget2D::SetInterlayer(Interlayer interlayer)
{
	if (tagDraw) return;
	this->interlayer = interlayer;
}

// 获取当前绘制模式
DxTarget2D::Interlayer DxTarget2D::GetInterlayer() const
{
	return interlayer;
}

// 开始绘制
bool DxTarget2D::BeginDraw()
{
	if (!pTarget) return false;
	if (tagDraw) return false;
	tagDraw = true;
	if (interlayer == DIRECT2D_ON_GDI) FlushGDI();
	pTarget->BeginDraw();
	return true;
}

// 结束绘制
HRESULT DxTarget2D::EndDraw()
{
	if (!pTarget) return E_FAIL;
	if (!tagDraw) return E_FAIL;
	tagDraw = false;
	HRESULT hr = pTarget->EndDraw();
	if (interlayer == GDI_ON_DIRECT2D) FlushGDI();
	return hr;
}

// 清除渲染目标
void DxTarget2D::Clear(D2D1_COLOR_F Color)
{
	if (!pTarget) return;
	pTarget->Clear(Color);
}

// 填充位图
void DxTarget2D::FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect, FLOAT alpha)
{
	if (!pTarget) return;
	if (srcRect.left == 0 && srcRect.right == 0 && srcRect.top == 0 && srcRect.bottom == 0)
	{
		srcRect.right = pBitmap->GetSize().width;
		srcRect.bottom = pBitmap->GetSize().height;
	}
	srcRect = DxGCalcFillBitmapSourceRect(srcRect, dstRect);
	pTarget->DrawBitmap(pBitmap, dstRect, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
}

// 放置位图
D2D1_RECT_F DxTarget2D::PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect, FLOAT alpha)
{
	if (!pTarget) return { 0 };
	if (srcRect.left == 0 && srcRect.right == 0 && srcRect.top == 0 && srcRect.bottom == 0)
	{
		srcRect.right = pBitmap->GetSize().width;
		srcRect.bottom = pBitmap->GetSize().height;
	}
	dstRect = DxGCalcPutBitmapTargetRect(srcRect, dstRect);
	pTarget->DrawBitmap(pBitmap, dstRect, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
	return dstRect;
}

// 绘制进度条
void DxTarget2D::DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor)
{
	if (!pTarget) return;
	ID2D1SolidColorBrush* pBrush = nullptr;

	// 绘制背景颜色
	pTarget->CreateSolidColorBrush(BackColor, &pBrush);
	if (!pBrush) return;
	pTarget->FillRectangle(Rect, pBrush);
	pBrush->Release();

	// 计算前景颜色的矩形区域并绘制
	Rect.right = Rect.left + ((Rect.right - Rect.left) * Percentage);
	pTarget->CreateSolidColorBrush(FrontColor, &pBrush);
	if (!pBrush) return;
	pTarget->FillRectangle(Rect, pBrush);
	pBrush->Release();
	pBrush = nullptr;
}

// 使用矩形引用绘制按钮
bool DxTarget2D::DrawButton(D2D1_RECT_F& Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, FLOAT alpha, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor, D2D1_COLOR_F BorderColor)
{
	// 调用重载的 DrawButton 函数，传递矩形的地址
	return DrawButton(&Rect, szText, TextFormat, pBackgroundBitmap, alpha, FrontColor, BackColor, BorderColor);
}

// 使用矩形指针绘制按钮
bool DxTarget2D::DrawButton(D2D1_RECT_F* Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, FLOAT alpha, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor, D2D1_COLOR_F BorderColor)
{
	if (!pTarget || !szText || !TextFormat) return false;

	UINT32 textLength = UINT32(wcslen(szText));
	float fFontSize = TextFormat->FontSize();
	IDWriteTextLayout* pLayout = nullptr;
	ID2D1SolidColorBrush* pBrush = nullptr;
	DWRITE_TEXT_METRICS textMetrics = { 0 };
	DWRITE_TEXT_RANGE textRange = { 0, textLength };

	// 创建文本布局
	HRESULT hr = TextFormat->WriteFactory()->CreateTextLayout(szText, textLength, TextFormat->TextFormat(),
		Rect->right - Rect->left, Rect->bottom - Rect->top, &pLayout);
	if (FAILED(hr)) return false;

	// 设置文本对齐方式
	hr = pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr)) {
		pLayout->Release();
		return false;
	}

	// 绘制背景
	if (pBackgroundBitmap) {
		*Rect = PutBitmap(pBackgroundBitmap, *Rect, { 0 }, alpha);
	}
	else if (BackColor.a != 0.0f) {
		hr = pTarget->CreateSolidColorBrush(BackColor, &pBrush);
		if (SUCCEEDED(hr)) {
			pTarget->FillRectangle(Rect, pBrush);
			pBrush->Release();
			pBrush = nullptr;
		}
		else {
			pLayout->Release();
			return false;
		}
	}

	// 绘制边框
	if (BorderColor.a != 0.0f) {
		hr = pTarget->CreateSolidColorBrush(BorderColor, &pBrush);
		if (SUCCEEDED(hr)) {
			pTarget->DrawRectangle(Rect, pBrush);
			pBrush->Release();
			pBrush = nullptr;
		}
		else {
			pLayout->Release();
			return false;
		}
	}

	// 调整字体大小以适应按钮大小
	while (SUCCEEDED(hr)) {
		hr = pLayout->GetMetrics(&textMetrics);
		if (FAILED(hr)) break;
		if (textMetrics.widthIncludingTrailingWhitespace > Rect->right - Rect->left ||
			textMetrics.height > Rect->bottom - Rect->top) {
			if (fFontSize <= 8) {
				hr = E_FAIL;
				break;
			}
			fFontSize -= 2;
			hr = pLayout->SetFontSize(fFontSize, textRange);
		}
		else {
			break;
		}
	}

	// 绘制文本
	if (SUCCEEDED(hr)) {
		hr = pTarget->CreateSolidColorBrush(FrontColor, &pBrush);
		if (SUCCEEDED(hr)) {
			float vstart = ((Rect->bottom - Rect->top) - textMetrics.height) / 2;
			pTarget->DrawTextLayout({ Rect->left, Rect->top + vstart }, pLayout, pBrush);
			pBrush->Release();
		}
	}

	pLayout->Release();
	return SUCCEEDED(hr);
}

// 调整渲染目标的大小
HRESULT DxTarget2D::Resize(UINT Width, UINT Height)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(D2D1::SizeU(Width, Height));
}

// 调整渲染目标的大小
HRESULT DxTarget2D::Resize(const D2D1_SIZE_U& pixelSize)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(pixelSize);
}

// 调整渲染目标的大小
HRESULT DxTarget2D::Resize(const D2D1_SIZE_U* pixelSize)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(pixelSize);
}

// 返回渲染目标指针
DxTarget2D::D2DTarget* DxTarget2D::Target()
{
	return pTarget;
}

// 类型转换运算符，返回渲染目标指针
DxTarget2D::operator D2DTarget* ()
{
	return pTarget;
}

// 刷新 GDI
void DxTarget2D::FlushGDI() const
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

// DxImage 类的构造函数
DxImage::DxImage()
{
	pImage = nullptr; // 初始化指针为 nullptr
}

// DxImage 类的析构函数
DxImage::~DxImage()
{
	Release(); // 调用 Release 方法释放资源
}

// 从文件加载图像
HRESULT DxImage::Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath)
{
	if (!pImagingFactory) return E_FAIL;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICFormatConverter* pConverter = nullptr;

	// 创建解码器
	HRESULT hr = pImagingFactory->CreateDecoderFromFilename(
		szPath,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);
	if (SUCCEEDED(hr)) hr = pDecoder->GetFrame(0, &pSource);
	if (SUCCEEDED(hr)) hr = pImagingFactory->CreateFormatConverter(&pConverter);
	if (SUCCEEDED(hr)) {
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}
	if (SUCCEEDED(hr)) {
		// 创建 Direct2D 位图
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			&pImage
		);
	}
	if (pDecoder) pDecoder->Release();
	if (pSource) pSource->Release();
	if (pConverter) pConverter->Release();
	if (!SUCCEEDED(hr)) pImage = nullptr;
	return hr;
}

// 从资源加载图像
HRESULT DxImage::Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType)
{
	if (!pImagingFactory) return E_FAIL;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICFormatConverter* pConverter = nullptr;
	IWICStream* pStream = nullptr;
	HRSRC hRscBitmap = nullptr;
	HGLOBAL hGlobalBitmap = nullptr;
	void* pImage = nullptr;
	DWORD nImageSize = 0;

	// 查找资源
	hRscBitmap = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscBitmap ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		// 加载资源
		hGlobalBitmap = LoadResource(hModule, hRscBitmap);
		hr = hGlobalBitmap ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// 锁定资源以获取内存指针
		pImage = LockResource(hGlobalBitmap);
		hr = pImage ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// 计算资源大小
		nImageSize = SizeofResource(hModule, hRscBitmap);
		hr = nImageSize ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// 创建 WIC 流
		hr = pImagingFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr)) {
		// 初始化流
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImage),
			nImageSize
		);
	}
	if (SUCCEEDED(hr)) {
		// 创建解码器
		hr = pImagingFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}
	if (SUCCEEDED(hr)) hr = pDecoder->GetFrame(0, &pSource);
	if (SUCCEEDED(hr)) hr = pImagingFactory->CreateFormatConverter(&pConverter);
	if (SUCCEEDED(hr)) {
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}
	if (SUCCEEDED(hr)) {
		// 创建 Direct2D 位图
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			&this->pImage
		);
	}
	if (pDecoder) pDecoder->Release();
	if (pSource) pSource->Release();
	if (pConverter) pConverter->Release();
	if (pStream) pStream->Release();
	if (!SUCCEEDED(hr)) this->pImage = nullptr;
	return hr;
}

// 释放图像资源
void DxImage::Release()
{
	if (pImage) pImage->Release();
	pImage = nullptr;
}

// 返回位图指针
DxImage::Image* DxImage::Bitmap()
{
	return pImage;
}

// 返回位图指针的指针
DxImage::Image** DxImage::BitmapPtr()
{
	return &pImage;
}

// 类型转换运算符，返回位图指针
DxImage::operator Image* ()
{
	return pImage;
}

// 类型转换运算符，返回位图指针的指针
DxImage::operator Image** ()
{
	return &pImage;
}

// 计算位图的裁剪区域以保证位图在绘制到指定位置时不会拉伸
D2D1_RECT_F DxGCalcFillBitmapSourceRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect)
{
	FLOAT xS = targetRect.right - targetRect.left;
	FLOAT yS = targetRect.bottom - targetRect.top;
	FLOAT rS = xS / yS;
	FLOAT xB = bitmapRect.right - bitmapRect.left;
	FLOAT yB = bitmapRect.bottom - bitmapRect.top;
	FLOAT rB = xB / yB;
	D2D1_RECT_F rR = bitmapRect;
	if (rS > rB)
	{
		yB = xB / rS;
		rR.top = (rR.top + rR.bottom - yB) / 2.0f;
		rR.bottom = rR.top + yB;
	}
	else if (rS < rB)
	{
		xB = yB * rS;
		rR.left = (rR.left + rR.right - xB) / 2.0f;
		rR.right = rR.left + xB;
	}
	return rR;
}

// 计算位图的放置区域以保证位图在绘制到指定位置时不会拉伸
D2D1_RECT_F DxGCalcPutBitmapTargetRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect)
{
	FLOAT xS = targetRect.right - targetRect.left;
	FLOAT yS = targetRect.bottom - targetRect.top;
	FLOAT rS = xS / yS;
	FLOAT xB = bitmapRect.right - bitmapRect.left;
	FLOAT yB = bitmapRect.bottom - bitmapRect.top;
	FLOAT rB = xB / yB;
	D2D1_RECT_F rR = targetRect;
	if (rS > rB)
	{
		xS = yS * rB;
		rR.left = (rR.left + rR.right - xS) / 2.0f;
		rR.right = rR.left + xS;
	}
	else if (rS < rB)
	{
		yS = xS / rB;
		rR.top = (rR.top + rR.bottom - yS) / 2.0f;
		rR.bottom = rR.top + yS;
	}
	return rR;
}

// 计算位图的裁剪区域以保证位图在绘制到指定位置时不会拉伸
D2D1_RECT_U DxGCalcFillBitmapSourceRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect)
{
	UINT xS = targetRect.right - targetRect.left;
	UINT yS = targetRect.bottom - targetRect.top;
	UINT rS = UINT(FLOAT(xS) / yS);
	UINT xB = bitmapRect.right - bitmapRect.left;
	UINT yB = bitmapRect.bottom - bitmapRect.top;
	UINT rB = UINT(FLOAT(xB) / yB);
	D2D1_RECT_U rR = bitmapRect;
	if (rS > rB)
	{
		yB = UINT(FLOAT(xB) / rS);
		rR.top = UINT32(FLOAT(rR.top + rR.bottom - yB) / 2.0f);
		rR.bottom = rR.top + yB;
	}
	else if (rS < rB)
	{
		xB = yB * rS;
		rR.left = UINT32(FLOAT(rR.left + rR.right - xB) / 2.0f);
		rR.right = rR.left + xB;
	}
	return rR;
}

// 计算位图的放置区域以保证位图在绘制到指定位置时不会拉伸
D2D1_RECT_U DxGCalcPutBitmapTargetRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect)
{
	UINT xS = targetRect.right - targetRect.left;
	UINT yS = targetRect.bottom - targetRect.top;
	UINT rS = UINT(FLOAT(xS) / yS);
	UINT xB = bitmapRect.right - bitmapRect.left;
	UINT yB = bitmapRect.bottom - bitmapRect.top;
	UINT rB = UINT(FLOAT(xB) / yB);
	D2D1_RECT_U rR = targetRect;
	if (rS > rB)
	{
		xS = yS * rB;
		rR.left = UINT32(FLOAT(rR.left + rR.right - xS) / 2.0f);
		rR.right = rR.left + xS;
	}
	else if (rS < rB)
	{
		yS = UINT(FLOAT(xS) / rB);
		rR.top = UINT32(FLOAT(rR.top + rR.bottom - yS) / 2.0f);
		rR.bottom = rR.top + yS;
	}
	return rR;
}

// 获取系统 DPI 缩放比例
FLOAT DxGetDpiScaleForSystem()
{
	return FLOAT(GetDpiForSystem()) / USER_DEFAULT_SCREEN_DPI;
}

// 获取窗口 DPI 缩放比例
FLOAT DxGetDpiScaleForWindow(HWND hWnd)
{
	return FLOAT(GetDpiForWindow(hWnd)) / USER_DEFAULT_SCREEN_DPI;
}

// 像素转换为 DIP (设备独立像素)
FLOAT DxGPixelsToDips(UINT Pixels, FLOAT DpiScale)
{
	return FLOAT(Pixels) / DpiScale;
}

// DIP 转换为像素
UINT DxGDipsToPixels(FLOAT Dips, FLOAT DpiScale)
{
	return UINT(Dips * DpiScale);
}

// 从矩形计算尺寸
D2D1_SIZE_F DxGCalcSizeFormRect(D2D1_RECT_F Rect)
{
	return D2D1_SIZE_F({ Rect.right - Rect.left, Rect.bottom - Rect.top });
}

// 从矩形计算尺寸
D2D1_SIZE_U DxGCalcSizeFormRect(D2D1_RECT_U Rect)
{
	return D2D1_SIZE_U({ Rect.right - Rect.left, Rect.bottom - Rect.top });
}

// 将尺寸从像素转换为 DIP
D2D1_SIZE_F DxGSizePixelsToDips(D2D1_SIZE_U Pixels, FLOAT DpiScale)
{
	return D2D1_SIZE_F({
		Pixels.width / DpiScale,
		Pixels.height / DpiScale
		});
}

// 将矩形从像素转换为 DIP
D2D1_RECT_F DxGRectPixelsToDips(D2D1_RECT_U Pixels, FLOAT DpiScale)
{
	return D2D1_RECT_F({
		FLOAT(Pixels.left) / DpiScale,
		FLOAT(Pixels.top) / DpiScale,
		FLOAT(Pixels.right) / DpiScale,
		FLOAT(Pixels.bottom) / DpiScale
		});
}

// 将尺寸从 DIP 转换为像素
D2D1_SIZE_U DxGSizeDipsToPixels(D2D1_SIZE_F Dips, FLOAT DpiScale)
{
	return D2D1_SIZE_U({
		UINT(Dips.width * DpiScale),
		UINT(Dips.height * DpiScale)
		});
}

// 将矩形从 DIP 转换为像素
D2D1_RECT_U DxGRectDipsToPixels(D2D1_RECT_F Dips, FLOAT DpiScale)
{
	return D2D1_RECT_U({
		UINT(Dips.left * DpiScale),
		UINT(Dips.top * DpiScale),
		UINT(Dips.right * DpiScale),
		UINT(Dips.bottom * DpiScale)
		});
}

// 计算中心矩形
// 参数 Item: 项目的尺寸（宽度和高度）
// 参数 Canvas: 画布的矩形区域
// 返回值: 位于画布中心的矩形
D2D1_RECT_F DxGCalcCenterRect(D2D1_SIZE_F Item, D2D1_RECT_F Canvas)
{
	D2D1_RECT_F result{}; // 初始化结果矩形
	FLOAT cWidth = Canvas.right - Canvas.left; // 计算画布的宽度
	FLOAT cHeight = Canvas.bottom - Canvas.top; // 计算画布的高度
	if (cWidth <= Item.width) {
		result.left = Canvas.left; // 如果画布宽度小于或等于项目宽度，左边界与画布左边界对齐
		result.right = Canvas.right; // 右边界与画布右边界对齐
	}
	else {
		result.left = Canvas.left + ((cWidth - Item.width) / 2.0f); // 否则居中项目
		result.right = result.left + Item.width; // 计算右边界
	}
	if (cHeight <= Item.height) {
		result.top = Canvas.top; // 如果画布高度小于或等于项目高度，上边界与画布上边界对齐
		result.bottom = Canvas.bottom; // 下边界与画布下边界对齐
	}
	else {
		result.top = Canvas.top + ((cHeight - Item.height) / 2.0f); // 否则居中项目
		result.bottom = result.top + Item.height; // 计算下边界
	}
	return result; // 返回计算好的矩形
}

// 计算中心矩形
// 参数 Item: 项目的尺寸（宽度和高度）
// 参数 Canvas: 画布的矩形区域
// 返回值: 位于画布中心的矩形
D2D1_RECT_U DxGCalcCenterRect(D2D1_SIZE_U Item, D2D1_RECT_U Canvas)
{
	D2D1_RECT_U result{}; // 初始化结果矩形
	UINT cWidth = Canvas.right - Canvas.left; // 计算画布的宽度
	UINT cHeight = Canvas.bottom - Canvas.top; // 计算画布的高度
	if (cWidth <= Item.width) {
		result.left = Canvas.left; // 如果画布宽度小于或等于项目宽度，左边界与画布左边界对齐
		result.right = Canvas.right; // 右边界与画布右边界对齐
	}
	else {
		result.left = UINT32(Canvas.left + (FLOAT(cWidth - Item.width) / 2.0f)); // 否则居中项目
		result.right = result.left + Item.width; // 计算右边界
	}
	if (cHeight <= Item.height) {
		result.top = Canvas.top; // 如果画布高度小于或等于项目高度，上边界与画布上边界对齐
		result.bottom = Canvas.bottom; // 下边界与画布下边界对齐
	}
	else {
		result.top = UINT32(Canvas.top + (FLOAT(cHeight - Item.height) / 2.0f)); // 否则居中项目
		result.bottom = result.top + Item.height; // 计算下边界
	}
	return result; // 返回计算好的矩形
}

// 从 LPARAM 获取尺寸
// 参数 lParam: 包含尺寸信息的 LPARAM
// 返回值: 包含宽度和高度的 D2D1_SIZE_U 结构
D2D1_SIZE_U DxGetSizeFromlParam(LPARAM lParam)
{
	return D2D1::SizeU(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); // 从 lParam 中提取 X 和 Y 坐标，并返回尺寸
}

// 从 LPARAM 获取点
// 参数 lParam: 包含点信息的 LPARAM
// 返回值: 包含 X 和 Y 坐标的 D2D1_POINT_2U 结构
D2D1_POINT_2U DxGetPointFromlParam(LPARAM lParam)
{
	return D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); // 从 lParam 中提取 X 和 Y 坐标，并返回点
}

// 判断点是否在矩形内 (浮点数版本)
// 参数 Point: 要判断的点
// 参数 Rect: 矩形区域
// 返回值: 如果点在矩形内返回 true，否则返回 false
bool DxGIsPointInRect(D2D1_SIZE_F Point, D2D1_RECT_F Rect)
{
	return (Point.width > Rect.left && Point.width < Rect.right && Point.height > Rect.top && Point.height < Rect.bottom); // 判断点是否在矩形边界内
}

// 判断点是否在矩形内 (整数版本)
// 参数 Point: 要判断的点
// 参数 Rect: 矩形区域
// 返回值: 如果点在矩形内返回 true，否则返回 false
bool DxGIsPointInRect(D2D1_SIZE_U Point, D2D1_RECT_U Rect)
{
	return (Point.width > Rect.left && Point.width < Rect.right && Point.height > Rect.top && Point.height < Rect.bottom); // 判断点是否在矩形边界内
}