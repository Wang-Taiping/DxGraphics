#include "DxGraphics.h"
#include <wincodec.h>
#include <wincodecsdk.h>
#include <wtypes.h>
#include <windowsx.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

DxImagingFactory::DxImagingFactory()
{
	pImagingFactory = nullptr;
}

DxImagingFactory::~DxImagingFactory()
{
	Uninitialize();
}

HRESULT DxImagingFactory::Initialize()
{
	if (pImagingFactory) return S_OK;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pImagingFactory)
	);
	return hr;
}

void DxImagingFactory::Uninitialize()
{
	if (pImagingFactory) pImagingFactory->Release();
	pImagingFactory = nullptr;
}

DxImagingFactory::ImagingFactory* DxImagingFactory::Factory()
{
	return pImagingFactory;
}

DxImagingFactory::operator ImagingFactory* ()
{
	return pImagingFactory;
}

DxWriteFactory::DxWriteFactory()
{
	pWriteFactory = nullptr;
}

DxWriteFactory::~DxWriteFactory()
{
	Uninitialize();
}

HRESULT DxWriteFactory::Initialize()
{
	if (pWriteFactory) return E_FAIL;
	return DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pWriteFactory)
	);
}

void DxWriteFactory::Uninitialize()
{
	if (pWriteFactory) pWriteFactory->Release();
	pWriteFactory = nullptr;
}

DxWriteFactory::WriteFactory* DxWriteFactory::Factory()
{
	return pWriteFactory;
}

DxWriteFactory::operator WriteFactory* ()
{
	return pWriteFactory;
}

DxFontCollection::DxFontCollection()
{
	pMemoryLoader = nullptr;
	pFontCollection = nullptr;
	pWriteFactory = nullptr;
	lpszFontFamily = nullptr;
}

DxFontCollection::~DxFontCollection()
{
	Uninitialize();
}

HRESULT DxFontCollection::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath)
{
	if (pFontCollection) return E_FAIL;
	IDWriteFontSetBuilder1* pFSBuilder = nullptr;
	IDWriteFontSet* pFontSet = nullptr;
	IDWriteFontFile* pFontFile = nullptr;
	IDWriteFontCollection1* pFCollection = nullptr;
	IDWriteFontFamily* pFFamily = nullptr;
	IDWriteLocalizedStrings* pFFName = nullptr;
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

HRESULT DxFontCollection::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType)
{
	if (pFontCollection) return E_FAIL;
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
	hRscFont = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscFont ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		// Load the resource.
		hGlobalFont = LoadResource(hModule, hRscFont);
		hr = hGlobalFont ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// Lock it to get a system memory pointer.
		pFontData = LockResource(hGlobalFont);
		hr = pFontData ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// Calculate the size.
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

void DxFontCollection::Uninitialize()
{
	if (pFontCollection) pFontCollection->Release();
	if (pMemoryLoader) {
		pWriteFactory->UnregisterFontFileLoader(pMemoryLoader);
		pMemoryLoader->Release();
	}
	if (lpszFontFamily) delete[] lpszFontFamily;
	pFontCollection = nullptr;
	pMemoryLoader = nullptr;
	pWriteFactory = nullptr;
	lpszFontFamily = nullptr;
}

DxFontCollection::WriteFontCollection* DxFontCollection::FontCollection()
{
	return pFontCollection;
}

DxWriteFactory::WriteFactory* DxFontCollection::WriteFactory()
{
	return pWriteFactory;
}

LPCWSTR DxFontCollection::FontFamily() const
{
	return lpszFontFamily;
}

DxFontCollection::operator WriteFontCollection* ()
{
	return pFontCollection;
}

DxFontCollection::operator DxWriteFactory::WriteFactory* ()
{
	return pWriteFactory;
}

DxTextFormat::DxTextFormat()
{
	pTextFormat = nullptr;
	pWriteFactory = nullptr;
	fFontSize = 0.0f;
}

DxTextFormat::~DxTextFormat()
{
	Uninitialize();
}

HRESULT DxTextFormat::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName)
{
	this->pWriteFactory = pWriteFactory;
	this->fFontSize = fFontSize;
	return pWriteFactory->CreateTextFormat(
		szFontFamilyName ? szFontFamilyName : L"Microsoft YaHei",
		pFontCollection,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fFontSize,
		szLocaleName,
		&pTextFormat
	);
}

HRESULT DxTextFormat::Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName)
{
	return Initialize(pFontCollection->WriteFactory(), pFontCollection->FontFamily(), pFontCollection->FontCollection(), fFontSize, szLocaleName);
}

void DxTextFormat::Uninitialize()
{
	if (pTextFormat) pTextFormat->Release();
	pTextFormat = nullptr;
	fFontSize = 0.0f;
}

DxTextFormat::WriteTextFormat* DxTextFormat::TextFormat()
{
	return pTextFormat;
}

DxWriteFactory::WriteFactory* DxTextFormat::WriteFactory()
{
	return pWriteFactory;
}

float DxTextFormat::FontSize() const
{
	return fFontSize;
}

DxTextFormat::operator WriteTextFormat* ()
{
	return pTextFormat;
}

DxFactory2D::DxFactory2D()
{
	pFactory = nullptr;
}

DxFactory2D::~DxFactory2D()
{
	Uninitialize();
}

HRESULT DxFactory2D::Initialize()
{
	return D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pFactory);
}

void DxFactory2D::Uninitialize()
{
	if (pFactory) pFactory->Release();
	pFactory = nullptr;
}

DxFactory2D::D2DFactory* DxFactory2D::Factory()
{
	return pFactory;
}

DxFactory2D::operator D2DFactory* ()
{
	return pFactory;
}

DxTarget2D::DxTarget2D()
{
	pTarget = nullptr;
	hWnd = hWnd;
	interlayer = GDI_ON_DIRECT2D;
	tagDraw = false;
}

DxTarget2D::~DxTarget2D()
{
	Uninitialize();
}

HRESULT DxTarget2D::Initialize(DxFactory2D::D2DFactory* p2DFactory, HWND hWnd)
{
	if (pTarget) return E_FAIL;
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

void DxTarget2D::Uninitialize()
{
	if (pTarget) pTarget->Release();
	pTarget = nullptr;
	hWnd = nullptr;
	interlayer = GDI_ON_DIRECT2D;
	tagDraw = false;
}

void DxTarget2D::SetInterlayer(Interlayer interlayer)
{
	if (tagDraw) return;
	this->interlayer = interlayer;
}

DxTarget2D::Interlayer DxTarget2D::GetInterlayer() const
{
	return interlayer;
}

bool DxTarget2D::BeginDraw()
{
	if (!pTarget) return false;
	if (tagDraw) return false;
	tagDraw = true;
	if (interlayer == DIRECT2D_ON_GDI) FlushGDI();
	pTarget->BeginDraw();
	return true;
}

HRESULT DxTarget2D::EndDraw()
{
	if (!pTarget) return E_FAIL;
	if (!tagDraw) return E_FAIL;
	tagDraw = false;
	HRESULT hr = pTarget->EndDraw();
	if (interlayer == GDI_ON_DIRECT2D) FlushGDI();
	return hr;
}

void DxTarget2D::Clear(D2D1_COLOR_F Color)
{
	if (!pTarget) return;
	pTarget->Clear(Color);
}

void DxTarget2D::FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect, FLOAT alpha)
{
	if (!pTarget) return;
	if (srcRect.left == 0 && srcRect.right == 0 && srcRect.top == 0 && srcRect.bottom == 0)
	{
		srcRect.right = pBitmap->GetSize().width;
		srcRect.bottom = pBitmap->GetSize().height;
	}
	srcRect = CalcFillBitmapSourceRect(srcRect, dstRect);
	pTarget->DrawBitmap(pBitmap, dstRect, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
}

D2D1_RECT_F DxTarget2D::PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect, FLOAT alpha)
{
	if (!pTarget) return { 0 };
	if (srcRect.left == 0 && srcRect.right == 0 && srcRect.top == 0 && srcRect.bottom == 0)
	{
		srcRect.right = pBitmap->GetSize().width;
		srcRect.bottom = pBitmap->GetSize().height;
	}
	dstRect = CalcPutBitmapTargetRect(srcRect, dstRect);
	pTarget->DrawBitmap(pBitmap, dstRect, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
	return dstRect;
}

void DxTarget2D::DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor)
{
	if (!pTarget) return;
	ID2D1SolidColorBrush* pBrush = nullptr;
	pTarget->CreateSolidColorBrush(BackColor, &pBrush);
	if (!pBrush) return;
	pTarget->FillRectangle(Rect, pBrush);
	pBrush->Release();
	Rect.right = Rect.left + ((Rect.right - Rect.left) * Percentage);
	pTarget->CreateSolidColorBrush(FrontColor, &pBrush);
	if (!pBrush) return;
	pTarget->FillRectangle(Rect, pBrush);
	pBrush->Release();
	pBrush = nullptr;
}

bool DxTarget2D::DrawButton(D2D1_RECT_F& Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, FLOAT alpha, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor, D2D1_COLOR_F BorderColor)
{
	// 调用重载的 DrawButton 函数，传递矩形的地址
	return DrawButton(&Rect, szText, TextFormat, pBackgroundBitmap, alpha, FrontColor, BackColor, BorderColor);
}

bool DxTarget2D::DrawButton(D2D1_RECT_F* Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, FLOAT alpha, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor, D2D1_COLOR_F BorderColor)
{
	// 先使用IDWriteTextLayout计算结果，如果不会溢出则直接输出到窗口
	// 如果计算发现会溢出则使用Layout减小字号再次计算，直至不会溢出为止
	// 如果字号减小到预设的最小值还是会溢出就返回false
	if (!pTarget) return false;
	if (!szText || !TextFormat) return false;
	UINT32 textLength = UINT32(wcslen(szText));
	float fFontSize = TextFormat->FontSize();
	IDWriteTextLayout* pLayout = nullptr;
	ID2D1SolidColorBrush* pBrush = nullptr;
	DWRITE_TEXT_METRICS textMetrics = { 0 };
	DWRITE_TEXT_RANGE textRange = { 0, textLength };
	D2D1_SIZE_F size = { 0 };

	HRESULT hr = TextFormat->WriteFactory()->CreateTextLayout(
		szText,
		textLength,
		TextFormat->TextFormat(),
		Rect->right - Rect->left,
		Rect->bottom - Rect->top,
		&pLayout
	);
	if (SUCCEEDED(hr)) hr = pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	else return false;
	if (SUCCEEDED(hr)) {
		if (pBackgroundBitmap) *Rect = PutBitmap(pBackgroundBitmap, *Rect, { 0 }, alpha);
		else if (BackColor.a != 0.0f) {
			hr = pTarget->CreateSolidColorBrush(BackColor, &pBrush);
			if (SUCCEEDED(hr)) {
				pTarget->FillRectangle(Rect, pBrush);
				pBrush->Release();
				pBrush = nullptr;
			}
			else hr = E_FAIL;
		}
	}
	if (SUCCEEDED(hr) && BorderColor.a != 0.0f) {
		hr = pTarget->CreateSolidColorBrush(BorderColor, &pBrush);
		if (SUCCEEDED(hr)) {
			pTarget->DrawRectangle(Rect, pBrush);
			pBrush->Release();
			pBrush = nullptr;
		}
	}
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
		else break;
	}
	if (SUCCEEDED(hr)) hr = pTarget->CreateSolidColorBrush(FrontColor, &pBrush);
	if (SUCCEEDED(hr)) {
		float vstart = ((Rect->bottom - Rect->top) - textMetrics.height) / 2;
		pTarget->DrawTextLayout({ Rect->left, Rect->top + vstart }, pLayout, pBrush);
	}
	if (pLayout) pLayout->Release();
	if (pBrush) pBrush->Release();
	return SUCCEEDED(hr);
}

HRESULT DxTarget2D::Resize(UINT Width, UINT Height)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(D2D1::SizeU(Width, Height));
}

HRESULT DxTarget2D::Resize(const D2D1_SIZE_U& pixelSize)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(pixelSize);
}

HRESULT DxTarget2D::Resize(const D2D1_SIZE_U* pixelSize)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(pixelSize);
}

DxTarget2D::D2DTarget* DxTarget2D::Target()
{
	return pTarget;
}

DxTarget2D::operator D2DTarget* ()
{
	return pTarget;
}

void DxTarget2D::FlushGDI() const
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

DxImage::DxImage()
{
	pImage = nullptr;
}

DxImage::~DxImage()
{
	Release();
}

HRESULT DxImage::Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath)
{
	if (!pImagingFactory) return E_FAIL;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICFormatConverter* pConverter = nullptr;
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
		// Create a Direct2D bitmap from the WIC bitmap.
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
	hRscBitmap = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscBitmap ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		// Load the resource.
		hGlobalBitmap = LoadResource(hModule, hRscBitmap);
		hr = hGlobalBitmap ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// Lock it to get a system memory pointer.
		pImage = LockResource(hGlobalBitmap);
		hr = pImage ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// Calculate the size.
		nImageSize = SizeofResource(hModule, hRscBitmap);
		hr = nImageSize ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// Create a WIC stream to map onto the memory.
		hr = pImagingFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr)) {
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImage),
			nImageSize
		);
	}
	if (SUCCEEDED(hr)) {
		// Create a decoder for the stream.
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
		// Create a Direct2D bitmap from the WIC bitmap.
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

void DxImage::Release()
{
	if (pImage) pImage->Release();
	pImage = nullptr;
}

DxImage::Image* DxImage::Bitmap()
{
	return pImage;
}

DxImage::Image** DxImage::BitmapPtr()
{
	return &pImage;
}

DxImage::operator Image* ()
{
	return pImage;
}

DxImage::operator Image** ()
{
	return &pImage;
}

// 计算位图的裁剪区域以保证位图在绘制到指定位置时不会拉伸
D2D1_RECT_F CalcFillBitmapSourceRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect)
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
D2D1_RECT_F CalcPutBitmapTargetRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect)
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

D2D1_RECT_U CalcFillBitmapSourceRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect)
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

D2D1_RECT_U CalcPutBitmapTargetRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect)
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

FLOAT GetDpiScaleForSystem()
{
	return FLOAT(GetDpiForSystem()) / USER_DEFAULT_SCREEN_DPI;
}

FLOAT GetDpiScaleForWindow(HWND hWnd)
{
	return FLOAT(GetDpiForWindow(hWnd)) / USER_DEFAULT_SCREEN_DPI;
}

FLOAT PixelsToDips(UINT Pixels, FLOAT DpiScale)
{
	return FLOAT(Pixels) / DpiScale;
}

UINT DipsToPixels(FLOAT Dips, FLOAT DpiScale)
{
	return UINT(Dips * DpiScale);
}

D2D1_SIZE_F CalcSizeFormRect(D2D1_RECT_F Rect)
{
	return D2D1_SIZE_F({ Rect.right - Rect.left, Rect.bottom - Rect.top });
}

D2D1_SIZE_U CalcSizeFormRect(D2D1_RECT_U Rect)
{
	return D2D1_SIZE_U({ Rect.right - Rect.left, Rect.bottom - Rect.top });
}

D2D1_SIZE_F SizePixelsToDips(D2D1_SIZE_U Pixels, FLOAT DpiScale)
{
	return D2D1_SIZE_F({
		Pixels.width / DpiScale,
		Pixels.height / DpiScale
		});
}

D2D1_RECT_F RectPixelsToDips(D2D1_RECT_U Pixels, FLOAT DpiScale)
{
	return D2D1_RECT_F({
		FLOAT(Pixels.left) / DpiScale,
		FLOAT(Pixels.top) / DpiScale,
		FLOAT(Pixels.right) / DpiScale,
		FLOAT(Pixels.bottom) / DpiScale
		});
}

D2D1_SIZE_U SizeDipsToPixels(D2D1_SIZE_F Dips, FLOAT DpiScale)
{
	return D2D1_SIZE_U({
		UINT(Dips.width * DpiScale),
		UINT(Dips.height * DpiScale)
		});
}

D2D1_RECT_U RectDipsToPixels(D2D1_RECT_F Dips, FLOAT DpiScale)
{
	return D2D1_RECT_U({
		UINT(Dips.left * DpiScale),
		UINT(Dips.top * DpiScale),
		UINT(Dips.right * DpiScale),
		UINT(Dips.bottom * DpiScale)
		});
}

D2D1_RECT_F CalcCenterRect(D2D1_SIZE_F Item, D2D1_RECT_F Canvas)
{
	D2D1_RECT_F result{};
	FLOAT cWidth = Canvas.right - Canvas.left;
	FLOAT cHeight = Canvas.bottom - Canvas.top;
	if (cWidth <= Item.width) {
		result.left = Canvas.left;
		result.right = Canvas.right;
	}
	else {
		result.left = Canvas.left + ((cWidth - Item.width) / 2.0f);
		result.right = result.left + Item.width;
	}
	if (cHeight <= Item.height) {
		result.top = Canvas.top;
		result.bottom = Canvas.bottom;
	}
	else {
		result.top = Canvas.top + ((cHeight - Item.height) / 2.0f);
		result.bottom = result.top + Item.height;
	}
	return result;
}

D2D1_RECT_U CalcCenterRect(D2D1_SIZE_U Item, D2D1_RECT_U Canvas)
{
	D2D1_RECT_U result{};
	UINT cWidth = Canvas.right - Canvas.left;
	UINT cHeight = Canvas.bottom - Canvas.top;
	if (cWidth <= Item.width) {
		result.left = Canvas.left;
		result.right = Canvas.right;
	}
	else {
		result.left = UINT32(Canvas.left + (FLOAT(cWidth - Item.width) / 2.0f));
		result.right = result.left + Item.width;
	}
	if (cHeight <= Item.height) {
		result.top = Canvas.top;
		result.bottom = Canvas.bottom;
	}
	else {
		result.top = UINT32(Canvas.top + (FLOAT(cHeight - Item.height) / 2.0f));
		result.bottom = result.top + Item.height;
	}
	return result;
}

D2D1_SIZE_U GetSizeFromlParam(LPARAM lParam)
{
	return D2D1::SizeU(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

D2D1_POINT_2U GetPointFromlParam(LPARAM lParam)
{
	return D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

bool IsPointInRect(D2D1_SIZE_F Point, D2D1_RECT_F Rect)
{
	return (Point.width > Rect.left && Point.width < Rect.right && Point.height > Rect.top && Point.height < Rect.bottom);
}

bool IsPointInRect(D2D1_SIZE_U Point, D2D1_RECT_U Rect)
{
	return (Point.width > Rect.left && Point.width < Rect.right && Point.height > Rect.top && Point.height < Rect.bottom);
}
