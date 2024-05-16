#pragma once

#ifndef DXGRAPHICS_H
#define DXGRAPHICS_H

#include <d3d11_4.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#ifdef DXG_EXPORT
#undef DXG_EXPORT
#endif // DXG_EXPORT
#ifdef DXG_BUILD
#define DXG_EXPORT __declspec(dllexport)
#else
#define DXG_EXPORT __declspec(dllimport)
#endif // DXG_BUILD

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define CreateCentralWindowA(lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(0L, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowW(lpszClassName, lpszWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(0L, lpszClassName, lpszWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowExW(dwExStyle, lpszClassName, lpszWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(dwExStyle, lpszClassName, lpszWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#ifdef UNICODE
#define CreateCentralWindow		CreateCentralWindowW
#define CreateCentralWindowEx	CreateCentralWindowExW
#else
#define CreateCentralWindow		CreateCentralWindowA
#define CreateCentralWindowEx	CreateCentralWindowExA
#endif // !UNICODE

class DXG_EXPORT DxImagingFactory
{
public:
	using ImagingFactory = IWICImagingFactory;

	DxImagingFactory();
	~DxImagingFactory();
	DxImagingFactory(const DxImagingFactory&) = delete;
	DxImagingFactory& operator=(const DxImagingFactory&) = delete;
	HRESULT Initialize();
	void Uninitialize();
	ImagingFactory* Factory();
	operator ImagingFactory* ();

private:
	ImagingFactory* pImagingFactory;
};

class DXG_EXPORT DxImage
{
public:
	using Image = ID2D1Bitmap;

	DxImage();
	~DxImage();
	DxImage(const DxImage&) = delete;
	DxImage& operator=(const DxImage&) = delete;
	HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath);
	HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);
	void Release();
	Image* Bitmap();
	Image** BitmapPtr();
	operator Image* ();
	operator Image** ();

private:
	Image* pImage;
};

class DXG_EXPORT DxWriteFactory
{
public:
	using WriteFactory = IDWriteFactory5;

	DxWriteFactory();
	~DxWriteFactory();
	DxWriteFactory(const DxWriteFactory&) = delete;
	DxWriteFactory& operator=(const DxWriteFactory&) = delete;
	HRESULT Initialize();
	void Uninitialize();
	WriteFactory* Factory();
	operator WriteFactory* ();

private:
	WriteFactory* pWriteFactory;
};

class DXG_EXPORT DxFontCollection
{
public:
	using WriteMemoryLoader = IDWriteInMemoryFontFileLoader;
	using WriteFontCollection = IDWriteFontCollection1;

	DxFontCollection();
	~DxFontCollection();
	DxFontCollection(const DxFontCollection&) = delete;
	DxFontCollection& operator=(const DxFontCollection&) = delete;
	HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath);
	HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);
	void Release();
	WriteFontCollection* FontCollection();
	DxWriteFactory::WriteFactory* WriteFactory();
	LPCWSTR FontFamily();
	operator WriteFontCollection* ();
	operator DxWriteFactory::WriteFactory* ();

private:
	WriteMemoryLoader* pMemoryLoader;
	WriteFontCollection* pFontCollection;
	DxWriteFactory::WriteFactory* pWriteFactory;
	LPWSTR lpszFontFamily;
};

class DXG_EXPORT DxTextFormat
{
public:
	using WriteTextFormat = IDWriteTextFormat;

	DxTextFormat();
	~DxTextFormat();
	DxTextFormat(const DxTextFormat&) = delete;
	DxTextFormat& operator=(const DxTextFormat&) = delete;
	HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);
	HRESULT Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);
	void Release();
	WriteTextFormat* TextFormat();
	DxWriteFactory::WriteFactory* WriteFactory();
	float FontSize();
	operator WriteTextFormat* ();
	operator DxWriteFactory::WriteFactory* ();

private:
	WriteTextFormat* pTextFormat;
	float fFontSize;
	DxWriteFactory::WriteFactory* pWriteFactory;
};

class DXG_EXPORT DxDevice // 使用设备像素而非DIP像素
{
public:
	DxDevice();
	~DxDevice();
	DxDevice(const DxDevice&) = delete;
	DxDevice& operator=(const DxDevice&) = delete;
	void Initialize();
	void Uninitialize();
	ID3D11Device* D3Device();
	ID3D11DeviceContext* D3DContext();
	IDXGIDevice3* DXGIDevice();
	IDXGIAdapter* DXGIAdapter();
	IDXGIFactory2* DXGIFactory();
	operator ID3D11Device* ();
	operator ID3D11DeviceContext* ();
	operator IDXGIDevice3* ();
	operator IDXGIAdapter* ();
	operator IDXGIFactory2* ();

private:
	ID3D11Device* p3Device;
	ID3D11DeviceContext* p3DContext;
	IDXGIDevice3* pDXGIDevice;
	IDXGIAdapter* pDXGIAdapter;
	IDXGIFactory2* pDXGIFactory;
	D3D_FEATURE_LEVEL currentLevel;

	void ZeroData();
};

class DXG_EXPORT DxWndContent // 使用设备像素而非DIP像素
{
public:
	DxWndContent();
	~DxWndContent();
	DxWndContent(const DxWndContent&) = delete;
	DxWndContent& operator=(const DxWndContent&) = delete;
	void Initialize(DxDevice* Device, HWND hWnd);
	void Uninitialize();
	void BeginDraw();
	bool EndDraw();
	void Clear(D2D1_COLOR_F Color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
	void FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 });
	D2D1_RECT_F PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 });
	void DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor); // 0.0f ~ 100.0f
	bool DrawButton(D2D1_RECT_F Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White));
	HWND hWnd() const;
	//IDXGISwapChain1* DXGISwapChain();
	//IDXGISurface* DXGISurface();
	ID2D1Factory1* D2DFactory();
	ID2D1Device* D2Device();
	ID2D1DeviceContext* D2DContext();
	operator HWND();
	//operator IDXGISwapChain1* ();
	//operator IDXGISurface* ();
	operator ID2D1Factory1* ();
	operator ID2D1Device* ();
	operator ID2D1DeviceContext* ();

private:
	DxDevice* pDevice;
	HWND handle_hWnd;
	IDXGISwapChain1* pDXGISwapChain;
	IDXGISurface* pDXGISurface;
	ID2D1Factory1* p2DFactory;
	ID2D1Device* p2Device;
	ID2D1DeviceContext* p2DContext;
	ID2D1Bitmap1* pDrawBitmap;
	D2D1_BITMAP_PROPERTIES1 bitmapProperties;

	void ZeroData();
};

DXG_EXPORT ATOM RegisterWndClass(HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon = nullptr, HICON hIconSm = nullptr, HCURSOR hCursor = nullptr, LPCWSTR lpszMenuName = nullptr);

#endif // !DXGRAPHICS_H
