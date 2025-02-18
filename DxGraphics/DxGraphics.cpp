#include "DxGraphics.h"
#include <wincodec.h>
#include <wincodecsdk.h>
#include <wtypes.h>
#include <windowsx.h>

// ���� D2D1 �� DWrite ��
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// DxImagingFactory ��Ĺ��캯��
DxImagingFactory::DxImagingFactory()
{
	pImagingFactory = nullptr; // ��ʼ��ָ��Ϊ nullptr
}

// DxImagingFactory �����������
DxImagingFactory::~DxImagingFactory()
{
	Uninitialize(); // ���� Uninitialize �����ͷ���Դ
}

// ��ʼ�� WIC (Windows Imaging Component) ����
HRESULT DxImagingFactory::Initialize()
{
	if (pImagingFactory) return S_OK; // ����ѳ�ʼ������ֱ�ӷ��� S_OK
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, // WIC �������� ID
		NULL,                    // ��ʹ�þۺ�
		CLSCTX_INPROC_SERVER,    // �� DLL �����д���
		IID_PPV_ARGS(&pImagingFactory) // �ӿ� ID ��ָ����սӿ�ָ��ĵ�ַ
	);
	return hr; // ���س�ʼ�����
}

// �ͷ� WIC ������Դ
void DxImagingFactory::Uninitialize()
{
	if (pImagingFactory) pImagingFactory->Release(); // �ͷŽӿ�
	pImagingFactory = nullptr; // ��ָ����Ϊ nullptr
}

// ���� WIC ����ָ��
DxImagingFactory::ImagingFactory* DxImagingFactory::Factory()
{
	return pImagingFactory;
}

// ����ת������������� WIC ����ָ��
DxImagingFactory::operator ImagingFactory* ()
{
	return pImagingFactory;
}

// DxWriteFactory ��Ĺ��캯��
DxWriteFactory::DxWriteFactory()
{
	pWriteFactory = nullptr; // ��ʼ��ָ��Ϊ nullptr
}

// DxWriteFactory �����������
DxWriteFactory::~DxWriteFactory()
{
	Uninitialize(); // ���� Uninitialize �����ͷ���Դ
}

// ��ʼ�� DirectWrite ����
HRESULT DxWriteFactory::Initialize()
{
	if (pWriteFactory) return E_FAIL; // ����ѳ�ʼ�����򷵻� E_FAIL
	return DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, // ������
		__uuidof(IDWriteFactory),   // DirectWrite �����Ľӿ� ID
		reinterpret_cast<IUnknown**>(&pWriteFactory) // ���սӿ�ָ��ĵ�ַ
	);
}

// �ͷ� DirectWrite ������Դ
void DxWriteFactory::Uninitialize()
{
	if (pWriteFactory) pWriteFactory->Release(); // �ͷŽӿ�
	pWriteFactory = nullptr; // ��ָ����Ϊ nullptr
}

// ���� DirectWrite ����ָ��
DxWriteFactory::WriteFactory* DxWriteFactory::Factory()
{
	return pWriteFactory;
}

// ����ת������������� DirectWrite ����ָ��
DxWriteFactory::operator WriteFactory* ()
{
	return pWriteFactory;
}

// DxFontCollection ��Ĺ��캯��
DxFontCollection::DxFontCollection()
{
	pMemoryLoader = nullptr;   // ��ʼ��ָ��Ϊ nullptr
	pFontCollection = nullptr;
	pWriteFactory = nullptr;
	lpszFontFamily = nullptr;
}

// DxFontCollection �����������
DxFontCollection::~DxFontCollection()
{
	Uninitialize(); // ���� Uninitialize �����ͷ���Դ
}

// ���ļ���ʼ�����弯��
HRESULT DxFontCollection::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath)
{
	if (pFontCollection) return E_FAIL; // ����ѳ�ʼ�����򷵻� E_FAIL
	IDWriteFontSetBuilder1* pFSBuilder = nullptr;
	IDWriteFontSet* pFontSet = nullptr;
	IDWriteFontFile* pFontFile = nullptr;
	IDWriteFontCollection1* pFCollection = nullptr;
	IDWriteFontFamily* pFFamily = nullptr;
	IDWriteLocalizedStrings* pFFName = nullptr;

	// �������弯������
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

	// �ͷ���Դ
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

// ����Դ��ʼ�����弯��
HRESULT DxFontCollection::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType)
{
	if (pFontCollection) return E_FAIL; // ����ѳ�ʼ�����򷵻� E_FAIL
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

	// ������Դ
	hRscFont = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscFont ? S_OK : E_FAIL;

	if (SUCCEEDED(hr)) {
		// ������Դ
		hGlobalFont = LoadResource(hModule, hRscFont);
		hr = hGlobalFont ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// ������Դ�Ի�ȡ�ڴ�ָ��
		pFontData = LockResource(hGlobalFont);
		hr = pFontData ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// ������Դ��С
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

	// �ͷ���Դ
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

// �ͷ����弯����Դ
void DxFontCollection::Uninitialize()
{
	if (pFontCollection) pFontCollection->Release(); // �ͷ����弯��
	if (pMemoryLoader) {
		pWriteFactory->UnregisterFontFileLoader(pMemoryLoader); // ע�������ļ�������
		pMemoryLoader->Release(); // �ͷż�����
	}
	if (lpszFontFamily) delete[] lpszFontFamily; // �ͷ������������
	pFontCollection = nullptr;
	pMemoryLoader = nullptr;
	pWriteFactory = nullptr;
	lpszFontFamily = nullptr;
}

// �������弯��ָ��
DxFontCollection::WriteFontCollection* DxFontCollection::FontCollection()
{
	return pFontCollection;
}

// ���� DirectWrite ����ָ��
DxWriteFactory::WriteFactory* DxFontCollection::WriteFactory()
{
	return pWriteFactory;
}

// ���������������
LPCWSTR DxFontCollection::FontFamily() const
{
	return lpszFontFamily;
}

// ����ת����������������弯��ָ��
DxFontCollection::operator WriteFontCollection* ()
{
	return pFontCollection;
}

// ����ת������������� DirectWrite ����ָ��
DxFontCollection::operator DxWriteFactory::WriteFactory* ()
{
	return pWriteFactory;
}

// DxTextFormat ��Ĺ��캯��
DxTextFormat::DxTextFormat()
{
	pTextFormat = nullptr; // ��ʼ��ָ��Ϊ nullptr
	pWriteFactory = nullptr;
	fFontSize = 0.0f;
}

// DxTextFormat �����������
DxTextFormat::~DxTextFormat()
{
	Uninitialize(); // ���� Uninitialize �����ͷ���Դ
}

// ��ʼ���ı���ʽ����
HRESULT DxTextFormat::Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName)
{
	this->pWriteFactory = pWriteFactory;
	this->fFontSize = fFontSize;
	return pWriteFactory->CreateTextFormat(
		szFontFamilyName ? szFontFamilyName : L"Microsoft YaHei", // �����������
		pFontCollection, // ���弯��
		DWRITE_FONT_WEIGHT_REGULAR, // �����ϸ
		DWRITE_FONT_STYLE_NORMAL, // ������ʽ
		DWRITE_FONT_STRETCH_NORMAL, // ��������
		fFontSize, // �����С
		szLocaleName, // ������������
		&pTextFormat // �����ı���ʽ����ĵ�ַ
	);
}

// ʹ�����弯�ϳ�ʼ���ı���ʽ����
HRESULT DxTextFormat::Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName)
{
	return Initialize(pFontCollection->WriteFactory(), pFontCollection->FontFamily(), pFontCollection->FontCollection(), fFontSize, szLocaleName);
}

// �ͷ��ı���ʽ������Դ
void DxTextFormat::Uninitialize()
{
	if (pTextFormat) pTextFormat->Release(); // �ͷ��ı���ʽ����
	pTextFormat = nullptr;
	fFontSize = 0.0f;
}

// �����ı���ʽ����ָ��
DxTextFormat::WriteTextFormat* DxTextFormat::TextFormat()
{
	return pTextFormat;
}

// ���� DirectWrite ����ָ��
DxWriteFactory::WriteFactory* DxTextFormat::WriteFactory()
{
	return pWriteFactory;
}

// ���������С
float DxTextFormat::FontSize() const
{
	return fFontSize;
}

// ����ת��������������ı���ʽ����ָ��
DxTextFormat::operator WriteTextFormat* ()
{
	return pTextFormat;
}

// DxFactory2D ��Ĺ��캯��
DxFactory2D::DxFactory2D()
{
	pFactory = nullptr; // ��ʼ��ָ��Ϊ nullptr
}

// DxFactory2D �����������
DxFactory2D::~DxFactory2D()
{
	Uninitialize(); // ���� Uninitialize �����ͷ���Դ
}

// ��ʼ�� Direct2D ����
HRESULT DxFactory2D::Initialize()
{
	return D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pFactory);
}

// �ͷ� Direct2D ������Դ
void DxFactory2D::Uninitialize()
{
	if (pFactory) pFactory->Release(); // �ͷŹ�������
	pFactory = nullptr;
}

// ���� Direct2D ����ָ��
DxFactory2D::D2DFactory* DxFactory2D::Factory()
{
	return pFactory;
}

// ����ת������������� Direct2D ����ָ��
DxFactory2D::operator D2DFactory* ()
{
	return pFactory;
}

// DxTarget2D ��Ĺ��캯��
DxTarget2D::DxTarget2D()
{
	pTarget = nullptr; // ��ʼ��ָ��Ϊ nullptr
	hWnd = nullptr;
	interlayer = GDI_ON_DIRECT2D;
	tagDraw = false;
}

// DxTarget2D �����������
DxTarget2D::~DxTarget2D()
{
	Uninitialize(); // ���� Uninitialize �����ͷ���Դ
}

// ��ʼ�� Direct2D ��ȾĿ��
HRESULT DxTarget2D::Initialize(DxFactory2D::D2DFactory* p2DFactory, HWND hWnd)
{
	if (pTarget) return E_FAIL; // ����ѳ�ʼ�����򷵻� E_FAIL
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

// �ͷ� Direct2D ��ȾĿ����Դ
void DxTarget2D::Uninitialize()
{
	if (pTarget) pTarget->Release(); // �ͷ���ȾĿ�����
	pTarget = nullptr;
	hWnd = nullptr;
	interlayer = GDI_ON_DIRECT2D;
	tagDraw = false;
}

// ���û���ģʽ
void DxTarget2D::SetInterlayer(Interlayer interlayer)
{
	if (tagDraw) return;
	this->interlayer = interlayer;
}

// ��ȡ��ǰ����ģʽ
DxTarget2D::Interlayer DxTarget2D::GetInterlayer() const
{
	return interlayer;
}

// ��ʼ����
bool DxTarget2D::BeginDraw()
{
	if (!pTarget) return false;
	if (tagDraw) return false;
	tagDraw = true;
	if (interlayer == DIRECT2D_ON_GDI) FlushGDI();
	pTarget->BeginDraw();
	return true;
}

// ��������
HRESULT DxTarget2D::EndDraw()
{
	if (!pTarget) return E_FAIL;
	if (!tagDraw) return E_FAIL;
	tagDraw = false;
	HRESULT hr = pTarget->EndDraw();
	if (interlayer == GDI_ON_DIRECT2D) FlushGDI();
	return hr;
}

// �����ȾĿ��
void DxTarget2D::Clear(D2D1_COLOR_F Color)
{
	if (!pTarget) return;
	pTarget->Clear(Color);
}

// ���λͼ
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

// ����λͼ
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

// ���ƽ�����
void DxTarget2D::DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor)
{
	if (!pTarget) return;
	ID2D1SolidColorBrush* pBrush = nullptr;

	// ���Ʊ�����ɫ
	pTarget->CreateSolidColorBrush(BackColor, &pBrush);
	if (!pBrush) return;
	pTarget->FillRectangle(Rect, pBrush);
	pBrush->Release();

	// ����ǰ����ɫ�ľ������򲢻���
	Rect.right = Rect.left + ((Rect.right - Rect.left) * Percentage);
	pTarget->CreateSolidColorBrush(FrontColor, &pBrush);
	if (!pBrush) return;
	pTarget->FillRectangle(Rect, pBrush);
	pBrush->Release();
	pBrush = nullptr;
}

// ʹ�þ������û��ư�ť
bool DxTarget2D::DrawButton(D2D1_RECT_F& Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, FLOAT alpha, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor, D2D1_COLOR_F BorderColor)
{
	// �������ص� DrawButton ���������ݾ��εĵ�ַ
	return DrawButton(&Rect, szText, TextFormat, pBackgroundBitmap, alpha, FrontColor, BackColor, BorderColor);
}

// ʹ�þ���ָ����ư�ť
bool DxTarget2D::DrawButton(D2D1_RECT_F* Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap, FLOAT alpha, D2D1_COLOR_F FrontColor, D2D1_COLOR_F BackColor, D2D1_COLOR_F BorderColor)
{
	if (!pTarget || !szText || !TextFormat) return false;

	UINT32 textLength = UINT32(wcslen(szText));
	float fFontSize = TextFormat->FontSize();
	IDWriteTextLayout* pLayout = nullptr;
	ID2D1SolidColorBrush* pBrush = nullptr;
	DWRITE_TEXT_METRICS textMetrics = { 0 };
	DWRITE_TEXT_RANGE textRange = { 0, textLength };

	// �����ı�����
	HRESULT hr = TextFormat->WriteFactory()->CreateTextLayout(szText, textLength, TextFormat->TextFormat(),
		Rect->right - Rect->left, Rect->bottom - Rect->top, &pLayout);
	if (FAILED(hr)) return false;

	// �����ı����뷽ʽ
	hr = pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr)) {
		pLayout->Release();
		return false;
	}

	// ���Ʊ���
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

	// ���Ʊ߿�
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

	// ���������С����Ӧ��ť��С
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

	// �����ı�
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

// ������ȾĿ��Ĵ�С
HRESULT DxTarget2D::Resize(UINT Width, UINT Height)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(D2D1::SizeU(Width, Height));
}

// ������ȾĿ��Ĵ�С
HRESULT DxTarget2D::Resize(const D2D1_SIZE_U& pixelSize)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(pixelSize);
}

// ������ȾĿ��Ĵ�С
HRESULT DxTarget2D::Resize(const D2D1_SIZE_U* pixelSize)
{
	if (!pTarget) return E_FAIL;
	return pTarget->Resize(pixelSize);
}

// ������ȾĿ��ָ��
DxTarget2D::D2DTarget* DxTarget2D::Target()
{
	return pTarget;
}

// ����ת���������������ȾĿ��ָ��
DxTarget2D::operator D2DTarget* ()
{
	return pTarget;
}

// ˢ�� GDI
void DxTarget2D::FlushGDI() const
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

// DxImage ��Ĺ��캯��
DxImage::DxImage()
{
	pImage = nullptr; // ��ʼ��ָ��Ϊ nullptr
}

// DxImage �����������
DxImage::~DxImage()
{
	Release(); // ���� Release �����ͷ���Դ
}

// ���ļ�����ͼ��
HRESULT DxImage::Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath)
{
	if (!pImagingFactory) return E_FAIL;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICFormatConverter* pConverter = nullptr;

	// ����������
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
		// ���� Direct2D λͼ
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

// ����Դ����ͼ��
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

	// ������Դ
	hRscBitmap = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscBitmap ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		// ������Դ
		hGlobalBitmap = LoadResource(hModule, hRscBitmap);
		hr = hGlobalBitmap ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// ������Դ�Ի�ȡ�ڴ�ָ��
		pImage = LockResource(hGlobalBitmap);
		hr = pImage ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// ������Դ��С
		nImageSize = SizeofResource(hModule, hRscBitmap);
		hr = nImageSize ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) {
		// ���� WIC ��
		hr = pImagingFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr)) {
		// ��ʼ����
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImage),
			nImageSize
		);
	}
	if (SUCCEEDED(hr)) {
		// ����������
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
		// ���� Direct2D λͼ
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

// �ͷ�ͼ����Դ
void DxImage::Release()
{
	if (pImage) pImage->Release();
	pImage = nullptr;
}

// ����λͼָ��
DxImage::Image* DxImage::Bitmap()
{
	return pImage;
}

// ����λͼָ���ָ��
DxImage::Image** DxImage::BitmapPtr()
{
	return &pImage;
}

// ����ת�������������λͼָ��
DxImage::operator Image* ()
{
	return pImage;
}

// ����ת�������������λͼָ���ָ��
DxImage::operator Image** ()
{
	return &pImage;
}

// ����λͼ�Ĳü������Ա�֤λͼ�ڻ��Ƶ�ָ��λ��ʱ��������
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

// ����λͼ�ķ��������Ա�֤λͼ�ڻ��Ƶ�ָ��λ��ʱ��������
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

// ����λͼ�Ĳü������Ա�֤λͼ�ڻ��Ƶ�ָ��λ��ʱ��������
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

// ����λͼ�ķ��������Ա�֤λͼ�ڻ��Ƶ�ָ��λ��ʱ��������
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

// ��ȡϵͳ DPI ���ű���
FLOAT DxGetDpiScaleForSystem()
{
	return FLOAT(GetDpiForSystem()) / USER_DEFAULT_SCREEN_DPI;
}

// ��ȡ���� DPI ���ű���
FLOAT DxGetDpiScaleForWindow(HWND hWnd)
{
	return FLOAT(GetDpiForWindow(hWnd)) / USER_DEFAULT_SCREEN_DPI;
}

// ����ת��Ϊ DIP (�豸��������)
FLOAT DxGPixelsToDips(UINT Pixels, FLOAT DpiScale)
{
	return FLOAT(Pixels) / DpiScale;
}

// DIP ת��Ϊ����
UINT DxGDipsToPixels(FLOAT Dips, FLOAT DpiScale)
{
	return UINT(Dips * DpiScale);
}

// �Ӿ��μ���ߴ�
D2D1_SIZE_F DxGCalcSizeFormRect(D2D1_RECT_F Rect)
{
	return D2D1_SIZE_F({ Rect.right - Rect.left, Rect.bottom - Rect.top });
}

// �Ӿ��μ���ߴ�
D2D1_SIZE_U DxGCalcSizeFormRect(D2D1_RECT_U Rect)
{
	return D2D1_SIZE_U({ Rect.right - Rect.left, Rect.bottom - Rect.top });
}

// ���ߴ������ת��Ϊ DIP
D2D1_SIZE_F DxGSizePixelsToDips(D2D1_SIZE_U Pixels, FLOAT DpiScale)
{
	return D2D1_SIZE_F({
		Pixels.width / DpiScale,
		Pixels.height / DpiScale
		});
}

// �����δ�����ת��Ϊ DIP
D2D1_RECT_F DxGRectPixelsToDips(D2D1_RECT_U Pixels, FLOAT DpiScale)
{
	return D2D1_RECT_F({
		FLOAT(Pixels.left) / DpiScale,
		FLOAT(Pixels.top) / DpiScale,
		FLOAT(Pixels.right) / DpiScale,
		FLOAT(Pixels.bottom) / DpiScale
		});
}

// ���ߴ�� DIP ת��Ϊ����
D2D1_SIZE_U DxGSizeDipsToPixels(D2D1_SIZE_F Dips, FLOAT DpiScale)
{
	return D2D1_SIZE_U({
		UINT(Dips.width * DpiScale),
		UINT(Dips.height * DpiScale)
		});
}

// �����δ� DIP ת��Ϊ����
D2D1_RECT_U DxGRectDipsToPixels(D2D1_RECT_F Dips, FLOAT DpiScale)
{
	return D2D1_RECT_U({
		UINT(Dips.left * DpiScale),
		UINT(Dips.top * DpiScale),
		UINT(Dips.right * DpiScale),
		UINT(Dips.bottom * DpiScale)
		});
}

// �������ľ���
// ���� Item: ��Ŀ�ĳߴ磨��Ⱥ͸߶ȣ�
// ���� Canvas: �����ľ�������
// ����ֵ: λ�ڻ������ĵľ���
D2D1_RECT_F DxGCalcCenterRect(D2D1_SIZE_F Item, D2D1_RECT_F Canvas)
{
	D2D1_RECT_F result{}; // ��ʼ���������
	FLOAT cWidth = Canvas.right - Canvas.left; // ���㻭���Ŀ��
	FLOAT cHeight = Canvas.bottom - Canvas.top; // ���㻭���ĸ߶�
	if (cWidth <= Item.width) {
		result.left = Canvas.left; // ����������С�ڻ������Ŀ��ȣ���߽��뻭����߽����
		result.right = Canvas.right; // �ұ߽��뻭���ұ߽����
	}
	else {
		result.left = Canvas.left + ((cWidth - Item.width) / 2.0f); // ���������Ŀ
		result.right = result.left + Item.width; // �����ұ߽�
	}
	if (cHeight <= Item.height) {
		result.top = Canvas.top; // ��������߶�С�ڻ������Ŀ�߶ȣ��ϱ߽��뻭���ϱ߽����
		result.bottom = Canvas.bottom; // �±߽��뻭���±߽����
	}
	else {
		result.top = Canvas.top + ((cHeight - Item.height) / 2.0f); // ���������Ŀ
		result.bottom = result.top + Item.height; // �����±߽�
	}
	return result; // ���ؼ���õľ���
}

// �������ľ���
// ���� Item: ��Ŀ�ĳߴ磨��Ⱥ͸߶ȣ�
// ���� Canvas: �����ľ�������
// ����ֵ: λ�ڻ������ĵľ���
D2D1_RECT_U DxGCalcCenterRect(D2D1_SIZE_U Item, D2D1_RECT_U Canvas)
{
	D2D1_RECT_U result{}; // ��ʼ���������
	UINT cWidth = Canvas.right - Canvas.left; // ���㻭���Ŀ��
	UINT cHeight = Canvas.bottom - Canvas.top; // ���㻭���ĸ߶�
	if (cWidth <= Item.width) {
		result.left = Canvas.left; // ����������С�ڻ������Ŀ��ȣ���߽��뻭����߽����
		result.right = Canvas.right; // �ұ߽��뻭���ұ߽����
	}
	else {
		result.left = UINT32(Canvas.left + (FLOAT(cWidth - Item.width) / 2.0f)); // ���������Ŀ
		result.right = result.left + Item.width; // �����ұ߽�
	}
	if (cHeight <= Item.height) {
		result.top = Canvas.top; // ��������߶�С�ڻ������Ŀ�߶ȣ��ϱ߽��뻭���ϱ߽����
		result.bottom = Canvas.bottom; // �±߽��뻭���±߽����
	}
	else {
		result.top = UINT32(Canvas.top + (FLOAT(cHeight - Item.height) / 2.0f)); // ���������Ŀ
		result.bottom = result.top + Item.height; // �����±߽�
	}
	return result; // ���ؼ���õľ���
}

// �� LPARAM ��ȡ�ߴ�
// ���� lParam: �����ߴ���Ϣ�� LPARAM
// ����ֵ: ������Ⱥ͸߶ȵ� D2D1_SIZE_U �ṹ
D2D1_SIZE_U DxGetSizeFromlParam(LPARAM lParam)
{
	return D2D1::SizeU(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); // �� lParam ����ȡ X �� Y ���꣬�����سߴ�
}

// �� LPARAM ��ȡ��
// ���� lParam: ��������Ϣ�� LPARAM
// ����ֵ: ���� X �� Y ����� D2D1_POINT_2U �ṹ
D2D1_POINT_2U DxGetPointFromlParam(LPARAM lParam)
{
	return D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); // �� lParam ����ȡ X �� Y ���꣬�����ص�
}

// �жϵ��Ƿ��ھ����� (�������汾)
// ���� Point: Ҫ�жϵĵ�
// ���� Rect: ��������
// ����ֵ: ������ھ����ڷ��� true�����򷵻� false
bool DxGIsPointInRect(D2D1_SIZE_F Point, D2D1_RECT_F Rect)
{
	return (Point.width > Rect.left && Point.width < Rect.right && Point.height > Rect.top && Point.height < Rect.bottom); // �жϵ��Ƿ��ھ��α߽���
}

// �жϵ��Ƿ��ھ����� (�����汾)
// ���� Point: Ҫ�жϵĵ�
// ���� Rect: ��������
// ����ֵ: ������ھ����ڷ��� true�����򷵻� false
bool DxGIsPointInRect(D2D1_SIZE_U Point, D2D1_RECT_U Rect)
{
	return (Point.width > Rect.left && Point.width < Rect.right && Point.height > Rect.top && Point.height < Rect.bottom); // �жϵ��Ƿ��ھ��α߽���
}