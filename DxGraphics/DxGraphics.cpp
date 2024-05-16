#include "DxGraphics.h"
#include <wincodec.h>
#include <wincodecsdk.h>

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
	if (pImagingFactory) return true;
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
	Release();
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

void DxFontCollection::Release()
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

LPCWSTR DxFontCollection::FontFamily()
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
	Release();
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

void DxTextFormat::Release()
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

float DxTextFormat::FontSize()
{
	return fFontSize;
}

DxTextFormat::operator WriteTextFormat* ()
{
	return pTextFormat;
}

// 计算位图的裁剪区域以保证位图在绘制到指定位置时不会拉伸
static D2D1_RECT_F CalcBitmapFillRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect)
{
	FLOAT xS = targetRect.right - targetRect.left; // xBitmap
	FLOAT yS = targetRect.bottom - targetRect.top; // yBitmap
	FLOAT rS = xS / yS;
	FLOAT xB = bitmapRect.right - bitmapRect.left; // xScreen
	FLOAT yB = bitmapRect.bottom - bitmapRect.top; // yScreen
	FLOAT rB = xB / yB;
	D2D1_RECT_F rR = bitmapRect;
	if (rS > rB)
	{
		yB = xB / rS;
		rR.top = (rR.top + rR.bottom - yB) / 2;
		rR.bottom = rR.top + yB;
	}
	else if (rS < rB)
	{
		xB = yB * rS;
		rR.left = (rR.left + rR.right - xB) / 2;
		rR.right = rR.left + xB;
	}
	return rR;
}

// 计算位图的放置区域以保证位图在绘制到指定位置时不会拉伸
static D2D1_RECT_F CalcBitmapPutRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect)
{
	FLOAT xS = targetRect.right - targetRect.left; // xBitmap
	FLOAT yS = targetRect.bottom - targetRect.top; // yBitmap
	FLOAT rS = xS / yS;
	FLOAT xB = bitmapRect.right - bitmapRect.left; // xScreen
	FLOAT yB = bitmapRect.bottom - bitmapRect.top; // yScreen
	FLOAT rB = xB / yB;
	D2D1_RECT_F rR = targetRect;
	if (rS > rB)
	{
		xS = yS * rB;
		rR.left = (rR.left + rR.right - xS) / 2;
		rR.right = rR.left + xS;
	}
	else if (rS < rB)
	{
		yS = xS / rB;
		rR.top = (rR.top + rR.bottom - yS) / 2;
		rR.bottom = rR.top + yS;
	}
	return rR;
}

DxWndContent::DxWndContent()
{
	ZeroData();
}

DxWndContent::~DxWndContent()
{
	Uninitialize();
}

void DxWndContent::Initialize(DxDevice* Device, HWND hWnd)
{
	pDevice = Device;
	handle_hWnd = hWnd;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;
	pDevice->DXGIFactory()->CreateSwapChainForHwnd(pDevice->D3Device(), hWnd, &swapChainDesc, nullptr, nullptr, &pDXGISwapChain);
	pDXGISwapChain->ResizeBuffers(2, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), DXGI_FORMAT_UNKNOWN, 0);
	pDevice->DXGIDevice()->SetMaximumFrameLatency(1);
	//pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&p3DTexture2D));
	bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			GetDpiForWindow(hWnd),
			GetDpiForWindow(hWnd)
		);
	pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pDXGISurface));
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p2DFactory);
	p2DFactory->CreateDevice(pDevice->DXGIDevice(), &p2Device);
	p2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &p2DContext);
	p2DContext->CreateBitmapFromDxgiSurface(pDXGISurface, bitmapProperties, &pDrawBitmap);
	p2DContext->SetTarget(pDrawBitmap);
}

void DxWndContent::Uninitialize()
{
	if (pDrawBitmap) pDrawBitmap->Release();
	if (p2DContext) p2DContext->Release();
	if (p2Device) p2Device->Release();
	if (p2DFactory) p2DFactory->Release();
	if (pDXGISurface) pDXGISurface->Release();
	if (pDXGISwapChain) pDXGISwapChain->Release();
	ZeroData();
}

void DxWndContent::BeginDraw()
{
	if (!p2DContext) return;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(handle_hWnd, &ps);
	EndPaint(handle_hWnd, &ps);
	p2DContext->BeginDraw();
}

bool DxWndContent::EndDraw()
{
	HRESULT hr = p2DContext->EndDraw();
	if (SUCCEEDED(hr)) hr = pDXGISwapChain->Present(1, 0);
	return SUCCEEDED(hr);
}

void DxWndContent::Clear(D2D1_COLOR_F Color)
{
	p2DContext->Clear(Color);
}

void DxWndContent::FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect)
{
	if (srcRect.left == 0 && srcRect.right == 0 && srcRect.top == 0 && srcRect.bottom == 0)
	{
		srcRect.right = pBitmap->GetSize().width;
		srcRect.bottom = pBitmap->GetSize().height;
	}
	srcRect = CalcBitmapFillRect(srcRect, dstRect);
	p2DContext->DrawBitmap(pBitmap, dstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
}

D2D1_RECT_F DxWndContent::PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect)
{
	if (srcRect.left == 0 && srcRect.right == 0 && srcRect.top == 0 && srcRect.bottom == 0)
	{
		srcRect.right = pBitmap->GetSize().width;
		srcRect.bottom = pBitmap->GetSize().height;
	}
	dstRect = CalcBitmapPutRect(srcRect, dstRect);
	p2DContext->DrawBitmap(pBitmap, dstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
	return dstRect;
}

void DxWndContent::DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor)
{
	ID2D1SolidColorBrush* pBrush = nullptr;
	p2DContext->CreateSolidColorBrush(BackColor, &pBrush);
	if (!pBrush) return;
	p2DContext->FillRectangle(Rect, pBrush);
	pBrush->Release();
	Rect.right = Rect.left + ((Rect.right - Rect.left) * Percentage);
	p2DContext->CreateSolidColorBrush(FrontColor, &pBrush);
	if (!pBrush) return;
	p2DContext->FillRectangle(Rect, pBrush);
	pBrush->Release();
	pBrush = nullptr;
}

bool DxWndContent::DrawButton(D2D1_RECT_F Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor)
{
	// 先使用IDWriteTextLayout计算结果，如果不会溢出则直接输出到窗口
	// 如果计算发现会溢出则使用Layout减小字号再次计算，直至不会溢出为止
	// 如果字号减小到预设的最小值还是会溢出就返回false
	if (!szText || !TextFormat) return false;
	IDWriteTextLayout* pLayout = nullptr;
	HRESULT hr = TextFormat->WriteFactory()->CreateTextLayout(
		szText,
		wcslen(szText),
		TextFormat->TextFormat(),
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		&pLayout
	);
	pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	float fFontSize = TextFormat->FontSize();
	ID2D1SolidColorBrush* pBrush = nullptr;
	if (SUCCEEDED(hr)) {
		if (pBackgroundBitmap) FillBitmap(pBackgroundBitmap, Rect);
		else {
			hr = p2DContext->CreateSolidColorBrush(BackColor, &pBrush);
			if (pBrush) p2DContext->FillRectangle(Rect, pBrush);
			pBrush->Release();
		}
		hr = p2DContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);
		if (pBrush) p2DContext->DrawRectangle(Rect, pBrush);
		pBrush->Release();
	}
	if (SUCCEEDED(hr)) {
		DWRITE_TEXT_METRICS textMetrics = { 0 };
		DWRITE_TEXT_RANGE textRange = { 0, wcslen(szText) };
		D2D1_SIZE_F size = { 0 };
		while (true) {
			hr = pLayout->GetMetrics(&textMetrics);
			if (textMetrics.widthIncludingTrailingWhitespace > Rect.right - Rect.left ||
				textMetrics.height > Rect.bottom - Rect.top) {
				if (fFontSize <= 8) return false;
				fFontSize -= 8;
				hr = pLayout->SetFontSize(fFontSize, textRange);
			}
			else break;
		}
		float vstart = ((Rect.bottom - Rect.top) - textMetrics.height) / 2;
		p2DContext->CreateSolidColorBrush(FrontColor, &pBrush);
		if (pLayout && pBrush) p2DContext->DrawTextLayout({ Rect.left, Rect.top + vstart }, pLayout, pBrush);
		pBrush->Release();
		return true;
	}
	return false;
}

HWND DxWndContent::hWnd() const
{
	return handle_hWnd;
}

ID3D11Device* DxDevice::D3Device()
{
	return p3Device;
}

ID3D11DeviceContext* DxDevice::D3DContext()
{
	return p3DContext;
}

IDXGIDevice3* DxDevice::DXGIDevice()
{
	return pDXGIDevice;
}

IDXGIAdapter* DxDevice::DXGIAdapter()
{
	return pDXGIAdapter;
}

IDXGIFactory2* DxDevice::DXGIFactory()
{
	return pDXGIFactory;
}

ID2D1Factory1* DxWndContent::D2DFactory()
{
	return p2DFactory;
}

ID2D1Device* DxWndContent::D2Device()
{
	return p2Device;
}

ID2D1DeviceContext* DxWndContent::D2DContext()
{
	return p2DContext;
}

DxWndContent::operator HWND()
{
	return handle_hWnd;
}

DxDevice::operator ID3D11Device* ()
{
	return p3Device;
}

DxDevice::operator ID3D11DeviceContext* ()
{
	return p3DContext;
}

DxDevice::operator IDXGIDevice3* ()
{
	return pDXGIDevice;
}

DxDevice::operator IDXGIAdapter* ()
{
	return pDXGIAdapter;
}

DxDevice::operator IDXGIFactory2* ()
{
	return pDXGIFactory;
}

DxWndContent::operator ID2D1Factory1* ()
{
	return p2DFactory;
}

DxWndContent::operator ID2D1Device* ()
{
	return p2Device;
}

DxWndContent::operator ID2D1DeviceContext* ()
{
	return p2DContext;
}

void DxWndContent::ZeroData()
{
	p2DContext = nullptr;
	p2Device = nullptr;
	p2DFactory = nullptr;
	pDXGISurface = nullptr;
	pDXGISwapChain = nullptr;
	pDrawBitmap = nullptr;
	bitmapProperties = D2D1_BITMAP_PROPERTIES1();
}

DxDevice::DxDevice()
{
	ZeroData();
}

DxDevice::~DxDevice()
{
	Uninitialize();
}

void DxDevice::Initialize()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1
	};
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &p3Device, &currentLevel, &p3DContext);
	p3Device->QueryInterface(&pDXGIDevice);
	pDXGIDevice->GetAdapter(&pDXGIAdapter);
	pDXGIAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));
}

void DxDevice::Uninitialize()
{
	if (pDXGIAdapter) pDXGIAdapter->Release();
	if (pDXGIDevice) pDXGIDevice->Release();
	if (pDXGIFactory) pDXGIFactory->Release();
	if (p3Device) p3Device->Release();
	ZeroData();
}

void DxDevice::ZeroData()
{
	p3DContext = nullptr;
	p3Device = nullptr;
	pDXGIAdapter = nullptr;
	pDXGIDevice = nullptr;
	pDXGIFactory = nullptr;
	currentLevel = D3D_FEATURE_LEVEL();
}

ATOM RegisterWndClass(HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, LPCWSTR lpszMenuName)
{
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClassName;
	wcex.lpfnWndProc = lpfnWndProc;
	wcex.hIcon = hIcon;
	wcex.hIconSm = hIconSm;
	wcex.hCursor = hCursor ? hCursor : LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW));
	wcex.lpszMenuName = lpszMenuName;
	return RegisterClassExW(&wcex);
}
